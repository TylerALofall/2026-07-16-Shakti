#include <stdio.h>
#include <string.h>

#define MAX_PAIRS 2048
#define MAX_COLORS 128
#define MAX_OBJECTS 512

typedef struct {
    char color[64];
    char object[256];
} Pair;

static Pair pairs[MAX_PAIRS];
static char colors[MAX_COLORS][64];
static char objects[MAX_OBJECTS][256];
static unsigned pair_count, color_count, object_count;

static void trim_line(char *s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = '\0';
}

static int add_unique(char *table, unsigned *count, unsigned max, size_t width, const char *value) {
    for (unsigned i = 0; i < *count; ++i)
        if (!strcmp(table + i * width, value)) return 1;
    if (*count == max || strlen(value) >= width) return 0;
    strcpy(table + (*count)++ * width, value);
    return 1;
}

static void sort_strings(char *table, unsigned count, size_t width) {
    char hold[256];
    for (unsigned i = 1; i < count; ++i) {
        strcpy(hold, table + i * width);
        unsigned j = i;
        while (j && strcmp(hold, table + (j - 1) * width) < 0) {
            strcpy(table + j * width, table + (j - 1) * width);
            --j;
        }
        strcpy(table + j * width, hold);
    }
}

static int pair_exists(const char *color, const char *object) {
    for (unsigned i = 0; i < pair_count; ++i)
        if (!strcmp(pairs[i].color, color) && !strcmp(pairs[i].object, object)) return 1;
    return 0;
}

int main(int argc, char **argv) {
    char line[512], previous[384] = "";
    FILE *in, *color_out, *object_out, *receipt;
    unsigned missing = 0, duplicates = 0;
    if (argc != 5) {
        fprintf(stderr, "usage: %s COLOR_OBJECTS.tbl COLORS.tbl OBJECTS.tbl RECEIPT.txt\n", argv[0]);
        return 2;
    }
    in = fopen(argv[1], "rb");
    if (!in) return 3;
    if (!fgets(line, sizeof line, in)) { fclose(in); return 4; }
    trim_line(line);
    if (strcmp(line, "color|object")) { fclose(in); return 5; }
    while (fgets(line, sizeof line, in)) {
        char key[384];
        trim_line(line);
        char *bar = strchr(line, '|');
        if (!bar || strchr(bar + 1, '|') || pair_count == MAX_PAIRS) { fclose(in); return 6; }
        *bar++ = '\0';
        if (!*line || !*bar || strlen(line) >= sizeof pairs[0].color || strlen(bar) >= sizeof pairs[0].object) {
            fclose(in); return 7;
        }
        size_t color_len = strlen(line), object_len = strlen(bar);
        memcpy(key, line, color_len);
        key[color_len] = '\037';
        memcpy(key + color_len + 1, bar, object_len + 1);
        if (previous[0] && strcmp(previous, key) > 0) { fclose(in); return 8; }
        if (previous[0] && !strcmp(previous, key)) ++duplicates;
        strcpy(previous, key);
        strcpy(pairs[pair_count].color, line);
        strcpy(pairs[pair_count].object, bar);
        ++pair_count;
        if (!add_unique((char *)colors, &color_count, MAX_COLORS, sizeof colors[0], line) ||
            !add_unique((char *)objects, &object_count, MAX_OBJECTS, sizeof objects[0], bar)) {
            fclose(in); return 9;
        }
    }
    fclose(in);
    sort_strings((char *)colors, color_count, sizeof colors[0]);
    sort_strings((char *)objects, object_count, sizeof objects[0]);
    for (unsigned c = 0; c < color_count; ++c)
        for (unsigned o = 0; o < object_count; ++o)
            if (!pair_exists(colors[c], objects[o])) ++missing;
    if (duplicates || missing || pair_count != color_count * object_count) {
        fprintf(stderr, "REFUSED pairs=%u colors=%u objects=%u duplicates=%u missing=%u\n",
                pair_count, color_count, object_count, duplicates, missing);
        return 10;
    }
    color_out = fopen(argv[2], "wb");
    object_out = fopen(argv[3], "wb");
    receipt = fopen(argv[4], "wb");
    if (!color_out || !object_out || !receipt) return 11;
    fputs("color\n", color_out);
    for (unsigned i = 0; i < color_count; ++i) fprintf(color_out, "%s\n", colors[i]);
    fputs("object\n", object_out);
    for (unsigned i = 0; i < object_count; ++i) fprintf(object_out, "%s\n", objects[i]);
    fprintf(receipt, "input=%s\npairs=%u\ncolors=%u\nobjects=%u\nexpected=%u\nduplicates=0\nmissing=0\n",
            argv[1], pair_count, color_count, object_count, color_count * object_count);
    if (fclose(color_out) || fclose(object_out) || fclose(receipt)) return 12;
    printf("VALID pairs=%u = colors=%u x objects=%u\n", pair_count, color_count, object_count);
    return 0;
}
