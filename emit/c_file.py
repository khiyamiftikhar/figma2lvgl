# emit/c_file.py

class CFile:
    def __init__(self, header_filename):
        self.lines = []
        self.header_filename = header_filename

        self._emit_preamble()

    def _emit_preamble(self):
        self.add(f'#include "{self.header_filename}"')
        self.add('#include "ui_defs.h"')
        self.add("")

    def add(self, line=""):
        self.lines.append(line)

    def add_block(self, title, lines):
        self.add("")
        self.add("// ------------------------------")
        self.add(f"// {title}")
        self.add("// ------------------------------")
        for line in lines:
            self.add(line)

    def render(self):
        return "\n".join(self.lines) + "\n"
