# Overgod 1.0 — Open8 AmigaOS 3.x port

This port keeps the upstream Allegro 4 game and its original game assets.
Use the native AmiCradle Allegro4 1.1 SDK, which links to
`allegro4.library` 1.1 and `acgame.library` 1.3.

## Cross-build

Unpack the Allegro4 1.1 RC1 SDK (with `include/allegro.h` and
`lib/liballeg.a`). With the Open8 m68k toolchain on PATH:

```sh
make -f Makefile.open8 ALLEGRO4_SDK=/absolute/path/to/allegro4-1.1-rc1
```

This produces `build/open8/Overgod`, an AmigaOS `loadseg()` executable.
The build needs a C runtime supporting the native Allegro stubs,
math library and socket library. It uses a big-endian m68k target.

## Guest installation

Unpack the release archive into an Amiga drawer. Keep `Overgod`,
`overgod.cfg`, `gfx/` and `wavs/` together. Copy the supplied
`Libs/allegro4.library` and `Libs/acgame.library` to `LIBS:` if
suitable versions are not already installed. Launch from a Shell
after changing directory to the game drawer (resource paths are relative).
The Allegro4/ACGame integration supports indexed AGA or RTG screens.

## Source validation performed 17 Nov 2026

- Cross-built all 20 C sources with GFOpenAmigaGCC 16.2.0 Open8.
- Linked against the native Allegro4 1.1 RC1 SDK.
- Corrected particle colour buffers: `create_cloud` stores five ints,
  but several callers formerly supplied only three/four or uninitialized
  extra colour slots.
- Verified `gfx/data.dat` is an Allegro plain datafile (`slh.`).
- Inspected the availability of the game's bundled bitmap and WAV files.

**Not validated yet:** opening a guest screen, keyboard/mouse/controller,
audio, game menu, actual gameplay, exit/relaunch, memory leak and frame
timings on AmigaOS 3.2.3 or AROS. A build and an archive do not prove a
playable game.

Original source and media credits: see `readme.txt` and `licence.txt`.
