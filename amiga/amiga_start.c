/* Overgod for GFOS: start-up bits. The game opens gfx/, wavs/ and
 * overgod.cfg relative to the current drawer, so it runs in its own drawer
 * (PROGDIR:) and gives the shell its drawer back when it ends.
 * Copyright (c) 2026 AmiCradle Team. GPL-2 or later, as Overgod. */
#include <proto/dos.h>

unsigned long __stack = 256 * 1024;
static const char amiga_ver[] __attribute__((used)) =
  "$VER: Overgod 1.0 (16.11.2026)";

static BPTR old_dir;
static int moved;

static void __attribute__((constructor)) amiga_progdir (void)
{
  BPTR dir = GetProgramDir ();
  if (dir)
    {
      old_dir = CurrentDir (dir);
      moved = 1;
    }
}

static void __attribute__((destructor)) amiga_back (void)
{
  if (moved)
    CurrentDir (old_dir);
}
