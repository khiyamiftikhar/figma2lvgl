# code_buffer.py

class CodeBuffer:
    def __init__(self):
        self._lines = []

    def add(self, line=""):
        self._lines.append(line)

    def extend(self, lines):
        self._lines.extend(lines)

    def render(self):
        return "\n".join(self._lines) + "\n"
