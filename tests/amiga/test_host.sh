#!/bin/sh
# Overgod for GFOS, host tests: the path checks, then every data file and
# overgod.cfg read back by gfopenallegro's own code (fake acgame.library).
#   tests/amiga/test_host.sh A4_DIR     (gfopenallegro/allegro4)
# Copyright (c) 2026 AmiCradle Team. GPL-2 or later, as Overgod.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
A4=${1:?gfopenallegro/allegro4 directory}
T=${TMPDIR:-/tmp}/overgod-host-$$
mkdir -p "$T"
trap 'rm -rf "$T"' EXIT HUP INT TERM
python3 -m unittest discover -s "$ROOT/tests/amiga" -p 'test_paths.py'
CC=${HOSTCC:-cc}
SRCS=$(ls "$A4"/src/a4_*.c | grep -v -e a4_os_amiga -e a4_amiga)
"$CC" -std=c99 -D_DEFAULT_SOURCE -w -O1 -I"$A4/include" -I"$A4/src" -I"$A4/tests" \
  $SRCS "$A4"/tests/fake_acgame.c "$ROOT/tests/amiga/test_data.c" -o "$T/test_data"
"$T/test_data" "$ROOT" "$T"
echo "host tests: PASS"
