# Overgod for GFOS

Overgod 1.0 by Linley Henzell (GPL-2 or later), built for AmigaOS 3.2 /
GFOS on a 68040, 68060 or 68080. No FPU is needed.

A fast arcade shooter set in an abstract mind-space: fly through waves of
glowing creatures, collect symbols to build up your ship, and face the
guardians at the end of each stage. One or two players.

## What is in the drawer

- `Overgod`: the game.
- `gfx/`: the pictures and the font datafile (`gfx/data.dat`).
- `wavs/` and `wavs/beat/`: the sounds and the music's drum samples.
- `overgod.cfg`: your settings, keys and unlocked ships. The game writes
  it back when you change them. Updates keep yours.
- `Libs/allegro4.library` and `Libs/acgame.library`: Allegro 4 for GFOS
  (gfopenallegro) on ACGame. The game opens them from its own `Libs/`.

## Playing

The keys are set in the game's Options menu. `Resolution` in
`overgod.cfg` picks the screen: 0 = 640x480 on its own screen,
1 = 800x600 on its own screen, 2 = 640x480 in a window (the default),
3 = 800x600 in a window.

## How the port is made

- The code is upstream's, built with 68040 soft float against
  gfopenallegro's Allegro 4 (`amiga/Makefile.amigaos`). Upstream's
  `xpart()`/`ypart()` are gnu89 `inline` functions, so it builds with
  `-fgnu89-inline`.
- Upstream wrote some paths the DOS way (`gfx//title.bmp`,
  `.//wavs//`). On AmigaDOS `//` is the parent drawer, so they are
  single slashes here.
- The game opens its files relative to the current drawer, so it moves
  into its own drawer (`PROGDIR:`) when it starts (`amiga/amiga_start.c`).
- Sound is silent for now: gfopenallegro plays samples once ACGame has
  audio.

## Tests

`tests/amiga/test_host.sh` checks that every file the code opens is in the
drawer under that name, then reads `gfx/data.dat`, every bitmap and
every sample, and `overgod.cfg` (read, change, write, re-read) through
gfopenallegro's own code. `package/build-amiga.sh` runs it before each
build.
