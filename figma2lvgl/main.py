"""figma2lvgl command line: Figma XML (FigML) + PNG images -> LVGL 9 C sources.

The steps are described in figma2lvgl-design, docs/design/09_cli_execution_flow.md.
Steps 1-6 only read inputs. Steps 7-13 write ui_src/ through core/output.py,
so a failed run never destroys the last good output.
"""

import sys
import shutil
import subprocess
import argparse
import logging
from pathlib import Path

from figma2lvgl.core.xml_input import read_design, screen_frames
from figma2lvgl.core.figma_parser import parse_design
from figma2lvgl.core import naming
from figma2lvgl.core.naming import NamingError
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.core.utils.utils import write_file
from figma2lvgl.core.config_writer import write_ui_config
from figma2lvgl.core import output
from figma2lvgl.tools.image_converter import convert_images

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

# LVGL's image converter, bundled unmodified from LVGL v9.3.0 (see the
# header of that file for the tag, commit, SHA-256 and licence).
BUNDLED_LVGL_TOOL = Path(__file__).parent / "tools" / "LVGLImage.py"
STATIC_SRC = Path(__file__).parent / "static_src"


# -------------------------------------------------
# Step 9: check if ui_src already has content and ask the user
# -------------------------------------------------
def confirm_overwrite(ui_src: Path, auto_yes: bool = False) -> bool:
    """
    Returns True if it's safe to proceed (folder is fresh, empty,
    or user confirmed / --yes flag was passed).
    """
    if not ui_src.exists():
        return True

    subfolders = output.MANAGED
    has_content = any(
        (ui_src / sub).is_dir() and any((ui_src / sub).iterdir())
        for sub in subfolders
    )

    if not has_content:
        return True

    if auto_yes:
        logger.info("--yes flag set: overwriting existing output in %s", ui_src)
        return True

    print("\n==========================================")
    print(" WARNING: Output folder already has content")
    print("==========================================")
    print(f"  {ui_src}")
    print("\n  The following subfolders will be regenerated:")
    for sub in subfolders:
        folder = ui_src / sub
        if folder.is_dir():
            count = len(list(folder.iterdir()))
            print(f"    {sub}/  ({count} file(s))")
    print("  Other files in it are kept.")
    print()

    while True:
        answer = input("  Overwrite? [y/n]: ").strip().lower()
        if answer in ("y", "yes"):
            return True
        if answer in ("n", "no"):
            print("\nAborted. No files were changed.\n")
            return False
        print("  Please enter y or n.")


# -------------------------------------------------
# Step 5: required assets exist
# -------------------------------------------------
def required_assets(screens):
    names = set()
    for screen in screens:
        names.update(screen.get_required_assets())
    return sorted(names)


def validate_assets(screens, images_dir: Path) -> bool:
    missing = [
        images_dir / f"{asset_id}.png"
        for asset_id in required_assets(screens)
        if not (images_dir / f"{asset_id}.png").exists()
    ]

    if missing:
        print("\n==========================================")
        print(" ASSET VALIDATION FAILED")
        print("==========================================")
        print("The following image files are missing:\n")
        for m in missing:
            print("  -", m)
        print("\nPlease add the missing files and run again.")
        print("==========================================\n")
        return False

    return True


# -------------------------------------------------
# Step 5b: name spaces (design doc 10, section 5)
# -------------------------------------------------
BUNDLED_GUARDS = ("UI_STYLE_H", "UI_DEFS_H")


def check_names(screens, images_dir: Path) -> list[str]:
    """Check the planned output before anything is written: linker
    symbols, header-guard macros and file paths (case-insensitive) must each
    be produced once. Returns error messages."""
    pngs = sorted(p.stem for p in images_dir.glob("*.png"))
    problems = [
        f"image file {stem}.png would replace figma2lvgl's own {stem}.c; rename it "
        f"(a layer named '{stem}' uses image_{stem}.png)"
        for stem in pngs if stem.casefold() in naming.RESERVED_FILE_STEMS
    ]
    if problems:
        return problems

    m = naming.OutputManifest()
    for screen in screens:
        for name in screen.symbols.values():
            m.symbols.append((name, screen.name))
        for node, _path, _parent in screen.all_nodes_bfs():
            for name in node.symbols.values():
                m.symbols.append((name, node.layer))
        m.macros.append((f"UI_{screen.snake.upper()}_H", screen.name))
        m.files.append((f"include/ui_{screen.snake}.h", screen.name))
        m.files.append((f"src/ui_{screen.snake}.c", screen.name))
    for stem in pngs:
        m.symbols.append((stem, f"{stem}.png"))
        m.files.append((f"priv_src/{stem}.c", f"{stem}.png"))
    for guard in BUNDLED_GUARDS:
        m.macros.append((guard, "figma2lvgl"))
    for f in sorted(STATIC_SRC.glob("*.h")):
        m.files.append((f"priv_include/{f.name}", "figma2lvgl"))
    for f in sorted(STATIC_SRC.glob("*.c")):
        m.files.append((f"priv_src/{f.name}", "figma2lvgl"))
    m.files += [("priv_include/assets.h", "figma2lvgl"),
                ("priv_include/ui_config.h", "figma2lvgl")]
    return [d.message for d in naming.check_namespaces(m)]


# -------------------------------------------------
# --verify-compile (step 2 checks, step 11 run)
# -------------------------------------------------
def check_compile_setup(lvgl_includes):
    """Return an error message, or None if --verify-compile can run."""
    if not lvgl_includes:
        return ("--verify-compile needs --lvgl-include: the LVGL folder holding lvgl.h "
                "(and the folder holding lv_conf.h, if separate).")
    for inc in lvgl_includes:
        if not inc.is_dir():
            return f"--lvgl-include folder not found: {inc}"
    if not any((inc / "lvgl.h").is_file() for inc in lvgl_includes):
        return ("no lvgl.h found in the --lvgl-include folders: "
                + ", ".join(str(i) for i in lvgl_includes))
    if shutil.which("gcc") is None:
        return "--verify-compile needs gcc on PATH."
    return None


def verify_compile(ui_src: Path, lvgl_includes) -> None:
    """Syntax-check every generated .c against the real LVGL headers.
    Raises output.OutputError on the first failure."""
    c_files = sorted((ui_src / "src").glob("*.c")) + sorted((ui_src / "priv_src").glob("*.c"))
    base = [
        "gcc", "-std=c99", "-fsyntax-only",
        "-DLV_CONF_INCLUDE_SIMPLE", "-DLV_LVGL_H_INCLUDE_SIMPLE",
        f"-I{ui_src / 'include'}", f"-I{ui_src / 'priv_include'}",
    ] + [f"-I{inc}" for inc in lvgl_includes]
    for c_file in c_files:
        result = subprocess.run(base + [str(c_file)], capture_output=True, text=True)
        if result.returncode != 0:
            raise output.OutputError(
                f"compile check failed for {c_file.name}:\n{result.stderr}")
    logger.info("Compile check passed (%d file(s)).", len(c_files))


# -------------------------------------------------
# Argument parsing
# -------------------------------------------------
def parse_args(argv=None):
    parser = argparse.ArgumentParser(
        description="LVGL UI Generator — converts Figma XML + images to C/H source files.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  Basic:   figma2lvgl -x layout.xml\n"
            "  Full:    figma2lvgl -x layout.xml -i ./assets -d ./output\n"
            "  CI:      figma2lvgl -x layout.xml --yes\n"
            "  Checked: figma2lvgl -x layout.xml -y --verify-compile --lvgl-include ../lvgl\n"
        )
    )

    parser.add_argument(
        "-x", "--xml",
        required=True, metavar="PATH",
        help="(Required) Path to the Figma XML file exported by FigML."
    )
    parser.add_argument(
        "-i", "--images",
        metavar="PATH", default=None,
        help="Folder containing PNG image assets.\nDefault: same directory as the XML file."
    )
    parser.add_argument(
        "-d", "--dest",
        metavar="PATH", default=None,
        help="Destination folder where ui_src/ will be created.\nDefault: same directory as the XML file."
    )
    parser.add_argument(
        "-y", "--yes",
        action="store_true", default=False,
        help="Skip the overwrite prompt."
    )
    parser.add_argument(
        "--lvgl-tool",
        metavar="PATH", default=None, type=Path,
        help=(
            "Path to a different LVGLImage.py, for example one matching a newer LVGL.\n"
            "Default: the copy bundled with figma2lvgl (from LVGL v9.3.0)."
        )
    )
    parser.add_argument(
        "-f", "--color-format",
        choices=["RGB565", "RGB888", "ARGB8888", "L8"],
        default="RGB565", metavar="FMT",
        help=(
            "Pixel encoding for PNG image asset conversion.\n"
            "Must match your display hardware color depth.\n"
            "Options: RGB565, RGB888, ARGB8888, L8.  Default: RGB565."
        )
    )
    parser.add_argument(
        "--patch-esp-includes",
        action="store_true", default=False,
        help=(
            "[Deprecated] ESP-IDF include patching is now applied automatically "
            "to all generated image files. This flag is kept for backward "
            "compatibility and has no effect."
        )
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true", default=False,
        help="Enable debug-level logging."
    )
    parser.add_argument(
        "--verify-compile",
        action="store_true", default=False,
        help=(
            "Syntax-check the generated C with gcc against real LVGL headers\n"
            "before the output is replaced. Needs --lvgl-include and gcc.\n"
            "A failed check keeps the previous output."
        )
    )
    parser.add_argument(
        "--lvgl-include",
        metavar="PATH", type=Path, action="append", default=[],
        help=(
            "Include folder for --verify-compile: the LVGL folder holding lvgl.h,\n"
            "and the folder holding lv_conf.h if separate. Repeat for each folder."
        )
    )

    return parser.parse_args(argv)


# -------------------------------------------------
# Step 10: everything that goes into the new ui_src/
# -------------------------------------------------
def build_output(tree: output.OutputTree, screens, images_dir: Path,
                 lvgl_tool: Path, color_format: str) -> None:
    # 1. Images (LVGLImage.py runs as a separate process; its files are
    #    recorded by listing the folders it wrote to).
    priv_src = tree.dir("priv_src")
    priv_inc = tree.dir("priv_include")
    if not convert_images(images_dir, priv_src, priv_inc, lvgl_tool,
                          color_format=color_format):
        raise output.OutputError("image conversion failed (see the error above).")
    tree.record_tree("priv_src")
    tree.record_tree("priv_include")

    # 2. Static files.
    for header in sorted(STATIC_SRC.glob("*.h")):
        shutil.copy2(header, tree.path(f"priv_include/{header.name}"))
    for source in sorted(STATIC_SRC.glob("*.c")):
        shutil.copy2(source, tree.path(f"priv_src/{source.name}"))
    logger.info("Copied static files.")

    # 3. ui_config.h
    config = tree.path("priv_include/ui_config.h")
    largest = write_ui_config(screens, config.parent)
    logger.info("ui_config.h written (largest screen: %s)", largest)

    # 4. Screens.
    logger.info("Generating UI files...")
    for screen in screens:
        c_fname, h_fname, h_text, c_text = generate_screen(screen)
        write_file(tree.path(f"include/{h_fname}"), h_text)
        write_file(tree.path(f"src/{c_fname}"), c_text)
        logger.info("  %s  %s", c_fname, h_fname)


# -------------------------------------------------
# Step 11: what the new ui_src/ must contain
# -------------------------------------------------
def validate_output(tree: output.OutputTree, screens, assets) -> None:
    expected = [
        "priv_include/ui_config.h", "priv_include/ui_defs.h",
        "priv_include/ui_style.h", "priv_include/assets.h",
        "priv_src/ui_style.c",
    ]
    for screen in screens:
        c_fname, h_fname, _, _ = generate_screen(screen)
        expected += [f"src/{c_fname}", f"include/{h_fname}"]
    expected += [f"priv_src/{name}.c" for name in assets]
    tree.require(expected)

    declared = (tree.root / "priv_include" / "assets.h").read_text(encoding="utf-8")
    undeclared = [name for name in assets if f"LV_IMG_DECLARE({name});" not in declared]
    if undeclared:
        raise output.OutputError(
            "images used by the design are not declared in assets.h: " + ", ".join(undeclared))


# -------------------------------------------------
# Main
# -------------------------------------------------
def main(argv=None):
    args = parse_args(argv)

    # Step 1
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)

    # Step 2: resolve paths, check --verify-compile can run.
    xml_path = Path(args.xml).resolve()
    if not xml_path.is_file():
        logger.error("XML file not found: %s", xml_path)
        sys.exit(1)

    images_dir = Path(args.images).resolve() if args.images else xml_path.parent
    if not images_dir.is_dir():
        logger.error("Images directory not found: %s", images_dir)
        sys.exit(1)

    dest_dir = Path(args.dest).resolve() if args.dest else xml_path.parent
    ui_src = dest_dir / output.UI_SRC

    lvgl_includes = [p.resolve() for p in args.lvgl_include]
    if args.verify_compile:
        problem = check_compile_setup(lvgl_includes)
        if problem:
            logger.error(problem)
            sys.exit(1)
    elif lvgl_includes:
        logger.warning("--lvgl-include has no effect without --verify-compile.")

    if args.patch_esp_includes:
        logger.warning(
            "--patch-esp-includes is deprecated and has no effect. "
            "ESP-IDF include patching is now applied automatically."
        )

    # Step 3
    print("\n==========================================")
    print(" LVGL UI Generator")
    print("==========================================")
    print(f"  XML file   : {xml_path}")
    print(f"  Images dir : {images_dir}")
    print(f"  Output dir : {ui_src}")
    print("==========================================\n")

    # Step 4: parse XML.
    try:
        root = read_design(xml_path)   # keeps line breaks in text
    except Exception as e:
        logger.error("Failed to parse XML: %s", e)
        sys.exit(1)

    frames = screen_frames(root)   # a page's frames, or a frame exported alone
    if frames is None:
        logger.error("No <children> element found in XML.")
        sys.exit(1)

    if not frames:
        logger.error("No <Frame> nodes found in XML.")
        sys.exit(1)

    try:
        screens = parse_design(frames)
    except NamingError as e:
        logger.error("%s", e)
        sys.exit(1)

    # Step 5: assets.
    if not validate_assets(screens, images_dir):
        sys.exit(1)
    assets = required_assets(screens)

    # Step 5b: every generated name once per name space (doc 10, section 5).
    problems = check_names(screens, images_dir)
    if problems:
        for problem in problems:
            logger.error("%s", problem)
        sys.exit(1)

    # Step 6: LVGLImage.py
    if args.lvgl_tool is not None:
        lvgl_tool = args.lvgl_tool.resolve()
        if not lvgl_tool.is_file():
            logger.error("--lvgl-tool path not found: %s", lvgl_tool)
            sys.exit(1)
        logger.info("Using LVGLImage.py from --lvgl-tool: %s", lvgl_tool)
    else:
        lvgl_tool = BUNDLED_LVGL_TOOL

    # Steps 7-13: under the lock.
    try:
        with output.OutputLock(dest_dir):
            for message in output.recover(dest_dir):          # step 8
                logger.warning(message)

            if not confirm_overwrite(ui_src, auto_yes=args.yes):   # step 9
                sys.exit(0)

            def build(t):                                       # step 10
                build_output(t, screens, images_dir, lvgl_tool, args.color_format)

            def validate(t):                                    # step 11
                validate_output(t, screens, assets)
                if args.verify_compile:
                    verify_compile(t.root, lvgl_includes)

            for warning in output.write_output(dest_dir, build, validate):  # 12-13
                logger.warning(warning)
    except output.OutputError as e:
        logger.error("%s", e)
        if ui_src.exists():
            logger.error("The previous output in %s is unchanged.", ui_src)
        sys.exit(1)

    # Step 14
    src_dir, include_dir = ui_src / "src", ui_src / "include"
    priv_src, priv_inc = ui_src / "priv_src", ui_src / "priv_include"
    print("\n==========================================")
    print(" PIPELINE COMPLETED SUCCESSFULLY")
    print("==========================================")
    print(f"\n  ui_src/")
    print(f"    src/          ({len(list(src_dir.glob('*.c')))} .c files)")
    print(f"    include/      ({len(list(include_dir.glob('*.h')))} .h files)")
    print(f"    priv_src/     ({len(list(priv_src.glob('*.c')))} .c files)")
    print(f"    priv_include/ ({len(list(priv_inc.glob('*.h')))} .h files)")
    print()


if __name__ == "__main__":
    main()
