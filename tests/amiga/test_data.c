/* Overgod for GFOS: every file the game loads, read through gfopenallegro's
 * own code (host build, fake acgame.library), the way the game reads it:
 * gfx/data.dat, the gfx/ bitmaps, the wavs/ and wavs/beat/ samples, and
 * overgod.cfg through the config calls (read, change, write back, re-read).
 *   test_data GAME_DIR SCRATCH_DIR
 * Copyright (c) 2026 AmiCradle Team. GPL-2 or later, as Overgod. */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <allegro.h>

static int fails, checks;
#define CHECK(c) do { checks++; if (!(c)) { fails++; printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); } } while (0)

static int ends_with(const char *s, const char *e)
{
    size_t a = strlen(s), b = strlen(e);
    return a >= b && strcmp(s + a - b, e) == 0;
}

static int samples(const char *dirname)
{
    char path[1024];
    DIR *dir = opendir(dirname);
    struct dirent *de;
    int n = 0;
    CHECK(dir != NULL);
    while (dir && (de = readdir(dir)) != NULL) {
        SAMPLE *s;
        if (!ends_with(de->d_name, ".wav")) continue;
        snprintf(path, sizeof(path), "%s/%s", dirname, de->d_name);
        s = load_sample(path);
        CHECK(s != NULL);
        if (!s) { printf("  cannot load %s\n", path); continue; }
        CHECK(s->len > 0);
        destroy_sample(s);
        n++;
    }
    if (dir) closedir(dir);
    return n;
}

int main(int argc, char **argv)
{
    char path[1024], cfg[1024];
    DATAFILE *d;
    DIR *dir;
    struct dirent *de;
    int i, bmps = 0, wavs, beats;
    PALETTE pal;
    FILE *in, *out;

    if (argc < 3) return 2;
    CHECK(allegro_init() == 0);

    snprintf(path, sizeof(path), "%s/gfx/data.dat", argv[1]);
    d = load_datafile(path);
    CHECK(d != NULL);
    if (d) {
        for (i = 0; d[i].type != DAT_END; i++) CHECK(d[i].dat != NULL);
        CHECK(i > 0);
        printf("data.dat: %d objects\n", i);
        unload_datafile(d);
    }
    snprintf(path, sizeof(path), "%s/gfx", argv[1]);
    dir = opendir(path);
    CHECK(dir != NULL);
    while (dir && (de = readdir(dir)) != NULL) {
        BITMAP *b;
        if (!ends_with(de->d_name, ".bmp")) continue;
        snprintf(path, sizeof(path), "%s/gfx/%s", argv[1], de->d_name);
        b = load_bitmap(path, pal);
        CHECK(b != NULL);
        if (!b) { printf("  cannot load %s\n", de->d_name); continue; }
        CHECK(b->w > 0 && b->h > 0);
        destroy_bitmap(b);
        bmps++;
    }
    if (dir) closedir(dir);
    snprintf(path, sizeof(path), "%s/wavs", argv[1]);
    wavs = samples(path);
    snprintf(path, sizeof(path), "%s/wavs/beat", argv[1]);
    beats = samples(path);
    CHECK(bmps >= 14 && wavs >= 40 && beats >= 5);

    /* main.c: set_config_file("overgod.cfg"), reads Options, writes keys
     * back on exit. Work on a copy so the source file stays as it is. */
    snprintf(path, sizeof(path), "%s/overgod.cfg", argv[1]);
    snprintf(cfg, sizeof(cfg), "%s/overgod.cfg", argv[2]);
    in = fopen(path, "rb"); out = fopen(cfg, "wb");
    CHECK(in && out);
    if (in && out) { int c; while ((c = fgetc(in)) != EOF) fputc(c, out); }
    if (in) fclose(in);
    if (out) fclose(out);
    set_config_file(cfg);
    CHECK(get_config_int("Options", "Resolution", -1) == 2);
    CHECK(get_config_int("Options", "Sound_volume", -1) == 100);
    CHECK(get_config_int("Player1Keys", "Key0", -1) == 84);
    set_config_int("Player1Keys", "Key0", 75);
    flush_config_file();
    set_config_file(path);      /* away and back: re-read from the file */
    set_config_file(cfg);
    CHECK(get_config_int("Player1Keys", "Key0", -1) == 75);
    CHECK(get_config_int("Options", "Resolution", -1) == 2);
    CHECK(get_config_int("Misc", "Seed", -1) == 9871);

    printf("%d bitmaps, %d + %d samples; %d checks, %d failed\n", bmps, wavs, beats, checks, fails);
    return fails ? 1 : 0;
}
