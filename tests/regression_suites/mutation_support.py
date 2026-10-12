"""Deferred source edits used by mutation selection."""
from regression_support import HarnessError, Source

class MutationSource:
    """Describe edits without reading or matching source until resolve is called.

    Builder methods return new plans, retaining the original plan's edits.
    Resolution applies them in order; it neither writes the file nor caches text.
    """

    def __init__(self, path, text=None, edits=()):
        self.path = path
        self.initial = text
        self.edits = edits

    def edit(self, kind, *arguments):
        return MutationSource(self.path, self.initial, self.edits + ((kind, arguments),))

    def replace_in_function(self, function, old, new):
        return self.edit("function", function, old, new)

    def replace(self, old, new):
        """Queue replacement of every occurrence; resolution still requires a match."""
        return self.edit("text", old, new)

    def replace_unique(self, old, new):
        return self.edit("unique", old, new)

    def replace_in_region(self, start, end, old, new):
        """Queue one replacement between unique literal markers, including start but excluding end."""
        return self.edit("region", start, end, old, new)

    def resolve(self):
        """Return edited text, resolving a nested plan first or reading the file if no text was supplied."""
        text = self.initial
        if isinstance(text, MutationSource):
            text = text.resolve()
        if text is None:
            text = self.path.read_text()
        for kind, arguments in self.edits:
            if kind == "function":
                text = Source(self.path, text).replace_in_function(*arguments)
                continue
            old, new = arguments[-2:]
            begin, finish = 0, len(text)
            if kind == "region":
                start, end = arguments[:2]
                if text.count(start) != 1 or text.count(end) != 1:
                    raise HarnessError(f"{self.path}: mutation region no longer matches uniquely")
                begin, finish = text.index(start), text.index(end)
                if finish <= begin:
                    raise HarnessError(f"{self.path}: mutation region ends before it starts")
            region = text[begin:finish]
            matches = region.count(old)
            if not matches or (kind != "text" and matches != 1):
                raise HarnessError(f"{self.path}: mutation anchor matched {matches} times")
            text = text[:begin] + region.replace(old, new) + text[finish:]
        return text

def resolve_overrides(overrides):
    return {path: text.resolve() if isinstance(text, MutationSource) else text
            for path, text in overrides.items()}
