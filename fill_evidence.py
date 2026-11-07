#!/usr/bin/env python3
"""Put YOUR OWN strace output into the report (replaces the 'paste your output' markers).

Usage (after ./run_all.sh):
    pip install python-docx
    python3 fill_evidence.py CSA0405_Assignment1_Solution.docx CSA0405_Assignment1_Final.docx
"""
import re, sys, os
from docx import Document
from docx.shared import Pt
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.text.paragraph import Paragraph

src, dst = sys.argv[1], sys.argv[2]
here = os.path.dirname(os.path.abspath(__file__))
def read(rel):
    p = os.path.join(here, rel)
    if not os.path.exists(p):
        sys.exit(f"Missing {rel}. Run ./run_all.sh first (and install strace: sudo apt install strace).")
    return open(p, encoding="utf8", errors="replace").read().rstrip("\n")

trace = read("Task05/trace_demo.txt").splitlines()[:40]
stc = read("Task06/strace_c.txt")
m = re.search(r"^\s*[\d.]+\s+[\d.]+\s+\d+\s+(\d+)(?:\s+\d+)?\s+read\s*$", stc, re.M)
read_calls = m.group(1) if m else None
if not read_calls:
    sys.exit("Could not find the read line in Task06/strace_c.txt")

doc = Document(src)

def all_paragraphs(d):
    for p in d.element.body.iter(qn("w:p")):
        yield Paragraph(p, d)

def sub_in_runs(par, old, new):
    for r in par.runs:
        if old in r.text:
            r.text = r.text.replace(old, new); return True
    return False

def block_after(par, title, lines):
    """Insert a bold title line and a monospace block after paragraph `par`."""
    last = par._p
    def new_par(text, mono, bold=False):
        nonlocal last
        p = OxmlElement("w:p"); last.addnext(p); last = p
        para = Paragraph(p, par._parent)
        run = para.add_run(text)
        run.bold = bold
        if mono:
            run.font.name = "Courier New"; run.font.size = Pt(7.5)
            rpr = run._r.get_or_add_rPr(); rf = rpr.find(qn("w:rFonts"))
            rf.set(qn("w:eastAsia"), "Courier New"); rf.set(qn("w:cs"), "Courier New")
            para.paragraph_format.space_after = Pt(0)
            para.paragraph_format.left_indent = Pt(14)
        else:
            para.paragraph_format.space_before = Pt(4)
        return para
    new_par(title, False, True)
    for ln in lines: new_par(ln if ln else " ", True)

done = {"t5": False, "t6": False, "cell": False}
for par in list(all_paragraphs(doc)):
    t = par.text
    if "paste your own trace in the final report" in t and not done["t5"]:
        sub_in_runs(par, "; paste your own trace in the final report):",
                    "). My captured trace is shown first, then the annotated form of the same calls:")
        block_after(par, "My trace (Task05/trace_demo.txt, from strace -o trace_full.txt ./task05_demo):", trace)
        done["t5"] = True
    elif "[strace -c output]" in t and not done["t6"]:
        sub_in_runs(par, "Paste your own strace -c output here: [strace -c output].",
                    f"My strace -c output is shown below: the read() count is {read_calls}.")
        block_after(par, "strace -c ./task06 (Task06/strace_c.txt):", stc.splitlines())
        done["t6"] = True
    elif "paste your strace output" in t and not done["cell"]:
        sub_in_runs(par, "paste your strace output (total about 10,026 incl. 3 runs)",
                    f"{read_calls} (total for all three runs, from strace -c)")
        done["cell"] = True

left = [k for k, v in done.items() if not v]
if left: print("WARNING: markers not found for", left)
doc.save(dst)

# final check for unfilled placeholders
d2 = Document(dst)
bad = [p.text[:80] for p in all_paragraphs(d2) if re.search(r"paste your|\[strace|RegisterNumber|_{6,}", p.text)]
print("Saved", dst)
print("Unfilled placeholders:", bad if bad else "none")
