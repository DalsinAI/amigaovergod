"""Overgod for GFOS: every file the game opens is in the drawer, under the
name the code uses, and no path in the code has a "//" (on AmigaDOS that is
the parent drawer, not one slash as on DOS).
Copyright (c) 2026 AmiCradle Team. GPL-2 or later, as Overgod."""
import glob
import os
import re
import unittest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


def code(name):
    with open(os.path.join(ROOT, name), encoding="latin-1") as f:
        text = f.read()
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//[^\n]*", "", text)
    # developer-only blocks (FIX_FONT, GENERATE_SHIPS, DONT_COMPILE) are
    # never built: drop each #ifdef whose macro no file defines
    defined = set(re.findall(r"^#define\s+(\w+)", text, re.M))
    out, skip = [], []
    for line in text.split("\n"):
        m = re.match(r"\s*#\s*(ifdef|ifndef|if|else|endif)\b\s*(\w*)", line)
        if m and m.group(1) in ("ifdef", "ifndef", "if"):
            dead = m.group(1) == "ifdef" and m.group(2) not in defined
            skip.append(dead)
        elif m and m.group(1) == "else" and skip:
            skip[-1] = False  # the #else of a dead #ifdef is built
        elif m and m.group(1) == "endif" and skip:
            skip.pop()
        elif not any(skip):
            out.append(line)
    return "\n".join(out)


def all_code():
    return {os.path.basename(p): code(os.path.basename(p))
            for p in sorted(glob.glob(os.path.join(ROOT, "*.c")))}


class Paths(unittest.TestCase):
    def test_no_dos_double_slash(self):
        for name, text in all_code().items():
            for lit in re.findall(r'"([^"\n]*)"', text):
                if lit.startswith(("gfx", "wavs", "sound", ".")):
                    self.assertNotIn("//", lit, "%s: %s" % (name, lit))

    def test_gfx_files_exist(self):
        seen = 0
        for name, text in all_code().items():
            for lit in re.findall(r'"(gfx/[^"]+)"', text):
                seen += 1
                self.assertTrue(os.path.isfile(os.path.join(ROOT, lit)), lit)
        self.assertGreaterEqual(seen, 15)

    def test_samples_exist(self):
        text = code("sound.c")
        new = re.findall(r'load_new_sample_in\s*\([^,]+,\s*"([^"]+)"\)', text)
        beat = re.findall(r'load_beat_sample_in\s*\([^,]+,\s*"([^"]+)"\)', text)
        self.assertGreater(len(new), 20)
        self.assertGreater(len(beat), 5)
        for s in new:
            self.assertTrue(os.path.isfile(os.path.join(ROOT, "wavs", s + ".wav")), s)
        for s in beat:
            self.assertTrue(os.path.isfile(os.path.join(ROOT, "wavs", "beat", s + ".wav")), s)
        # the old sound/ samples are only loaded in commented-out code
        self.assertEqual(re.findall(r'\bload_sample_in\s*\([^,]+,\s*"', text), [])

    def test_names_fit_the_buffers(self):
        # sound.c builds "wavs/beat/<name>.wav" in a 50-byte buffer
        text = code("sound.c")
        for s in re.findall(r'load_\w+_sample_in\s*\([^,]+,\s*"([^"]+)"\)', text):
            self.assertLess(len("wavs/beat/" + s + ".wav"), 50, s)

    def test_default_config(self):
        with open(os.path.join(ROOT, "overgod.cfg"), encoding="latin-1") as f:
            cfg = f.read()
        self.assertRegex(cfg, r"\[Options\]")
        m = re.search(r"^Resolution = (\d)", cfg, re.M)
        self.assertIsNotNone(m)
        self.assertIn(m.group(1), "0123")


if __name__ == "__main__":
    unittest.main()
