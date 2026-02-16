# emit/h_file.py

class HFile:
    def __init__(self, guard):
        self.guard = guard
        self.lines = []

        self._emit_preamble()

    def _emit_preamble(self):
        self.lines.append(f"#ifndef {self.guard}")
        self.lines.append(f"#define {self.guard}")
        self.lines.append("")
        self.lines.append("#ifdef __cplusplus")
        self.lines.append('extern "C" {')
        self.lines.append("#endif")
        self.lines.append("")

    def add(self, line=""):
        self.lines.append(line)

    def add_prototype(self, proto):
        self.lines.append(proto)

    def close(self):
        self.lines.append("")
        self.lines.append("#ifdef __cplusplus")
        self.lines.append("}")
        self.lines.append("#endif")
        self.lines.append("")
        self.lines.append("#endif")

    def render(self):
        return "\n".join(self.lines) + "\n"
