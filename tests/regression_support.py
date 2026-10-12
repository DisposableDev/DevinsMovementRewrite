"""Lexical C extraction for host tests; unsupported layouts are harness errors."""
import json
from pathlib import Path
import re
from source_asset import read_text as read_source


class HarnessError(Exception):
    pass


def mask_literals(text):
    """Keep offsets/newlines, masking comments and quoted strings/characters."""
    pattern = r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
    return re.sub(pattern, lambda m: re.sub(r"[^\n]", " ", m[0]), text, flags=re.S)


def closing(mask, start, left, right):
    """Return the exclusive end of paired delimiters; start points at the opening one."""
    depth = 0
    for end in range(start, len(mask)):
        depth += (mask[end] == left) - (mask[end] == right)
        if depth == 0:
            return end + 1
    raise HarnessError(f"Unclosed {left!r} at character {start}")


class Source:
    """Search offset-preserving masked text, then extract and map the original text."""

    def __init__(self, path, text=None):
        self.path = Path(path).resolve()
        self.text = read_source(self.path) if text is None else text
        self.mask = mask_literals(self.text)

    def mapped(self, start, end):
        """Emit the original half-open character span with its source path and starting line."""
        line = self.text.count("\n", 0, start) + 1
        return f"#line {line} {json.dumps(str(self.path))}\n{self.text[start:end]}\n"

    def function_span(self, name):
        """Return one definition's half-open span, ignoring prototypes and masked literals."""
        pattern = r"^[ \t]*(?:[A-Za-z_]\w*[\s*]+)+" + re.escape(name) + r"\s*\("
        definition_spans = []
        for header_match in re.finditer(pattern, self.mask, re.M):
            parameters_end = closing(self.mask, header_match.end() - 1, "(", ")")
            body_match = re.match(r"\s*\{", self.mask[parameters_end:])
            if body_match:
                body_open = parameters_end + body_match.end() - 1
                definition_spans.append(
                    (header_match.start(), closing(self.mask, body_open, "{", "}"))
                )
        if len(definition_spans) != 1:
            raise HarnessError(
                f"{self.path}: expected one definition of {name}, found {len(definition_spans)}"
            )
        return definition_spans[0]

    def function(self, name):
        return self.mapped(*self.function_span(name))

    def declaration(self, kind, name):
        """Extract one bare braced declaration; map through its closing brace, then append a semicolon."""
        matches = list(re.finditer(r"\b" + kind + r"\s+" + re.escape(name) + r"\s*\{", self.mask))
        if len(matches) != 1:
            raise HarnessError(f"{self.path}: expected one {kind} {name}, found {len(matches)}")
        declaration_start = matches[0].start()
        body_end = closing(self.mask, matches[0].end() - 1, "{", "}")
        if not re.match(r"\s*;", self.mask[body_end:]):
            raise HarnessError(f"{self.path}: unsupported declaration for {name}")
        return self.mapped(declaration_start, body_end) + ";\n"

    def macros(self, pattern):
        """Select names on masked text; preserve original lines and follow their trailing backslashes."""
        chunks = []
        for macro_match in re.finditer(r"^[ \t]*#define[ \t]+(\w+)[^\n]*(?:\n|$)", self.mask, re.M):
            if not re.fullmatch(pattern, macro_match[1]):
                continue
            macro_end = macro_match.end()
            while self.text[:macro_end].rstrip("\n").endswith("\\"):
                next_newline = self.text.find("\n", macro_end)
                macro_end = len(self.text) if next_newline < 0 else next_newline + 1
                if macro_end == len(self.text):
                    break
            chunks.append(self.mapped(macro_match.start(), macro_end))
        if not chunks:
            raise HarnessError(f"{self.path}: no macros matching {pattern}")
        return "\n".join(chunks)

    def region(self, start_pattern, end_pattern):
        """Map from the unique start marker to before the unique end marker, in source order."""
        starts = list(re.finditer(start_pattern, self.mask, re.M))
        ends = list(re.finditer(end_pattern, self.mask, re.M))
        if len(starts) != 1 or len(ends) != 1 or starts[0].start() >= ends[0].start():
            raise HarnessError(f"{self.path}: source region changed; update the test adapter")
        return self.mapped(starts[0].start(), ends[0].start())

    def replace_in_function(self, name, old, new):
        """Return new source with one literal replacement inside the complete function definition."""
        start, end = self.function_span(name)
        definition_text = self.text[start:end]
        if definition_text.count(old) != 1:
            raise HarnessError(f"{self.path}: mutation no longer matches {name} uniquely")
        return self.text[:start] + definition_text.replace(old, new) + self.text[end:]


def animation_metadata(source):
    """Return (flags, startFrame, loopStart, loopEnd) for supported host playback.

    The host timing model omits animYTransDivisor. Reject headers outside its
    supported flags and frame limits rather than approximate another playback mode.
    """
    match = re.search(r"\banim_[0-9A-F]+\s*\[\s*\]\s*=\s*\{", source.mask)
    if not match:
        raise HarnessError(f"{source.path}: animation header not found")
    try:
        flags, _, start, loop_start, loop_end = [
            int(value.strip(), 0) for value in source.mask[match.end():].split(",")[:5]
        ]
    except ValueError as error:
        raise HarnessError(f"{source.path}: unsupported animation header") from error
    if flags != 1 or start != 0 or loop_start != 0 or loop_end < 3:
        raise HarnessError(f"{source.path}: host playback requires flags=1, start=loop_start=0, "
                           f"loop_end>=3; got {flags}, {start}, {loop_start}, {loop_end}")
    return flags, start, loop_start, loop_end
