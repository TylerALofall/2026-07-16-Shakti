#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAG 4096
#define MAX_TEXT 4096
#define MAX_NAME 64
#define MAX_VALUE 1024
#define MAX_DEPTH 64
#define MAX_ROWS 2048
#define MAX_LINE 4096

typedef struct {
    char name[MAX_NAME];
    char value[MAX_VALUE];
} Attr;

typedef struct {
    char name[MAX_NAME];
    Attr attrs[16];
    unsigned attr_count;
    int closing;
    int self_closing;
} Tag;

typedef struct {
    char token[128];
    char domain[64];
    char say[256];
    char word[256];
    char letters[512];
    char context[1024];
    char examples[1024];
    char object[256];
    char render_ref[512];
    char record_ref[512];
    char hex[32];
    char status[64];
    char paint_color[64];
    char lower[16];
    char upper[16];
    int ordinal;
    int sides;
    int vertices;
    int edges;
} Stone;

typedef struct {
    char key[384];
    char line[MAX_LINE];
    long number;
} Row;

static Row rows[MAX_ROWS];
static unsigned row_count;
static char stack_names[MAX_DEPTH][MAX_NAME];
static unsigned depth;
static Stone stone;
static char root_domain[64];
static unsigned refused_rows;
static unsigned unresolved_rows;

static void copy_text(char *dst, size_t cap, const char *src) {
    size_t n = strlen(src);
    if (n >= cap) n = cap - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static void trim(char *s) {
    size_t start = 0, end = strlen(s);
    while (start < end && isspace((unsigned char)s[start])) ++start;
    while (end > start && isspace((unsigned char)s[end - 1])) --end;
    if (start) memmove(s, s + start, end - start);
    s[end - start] = '\0';
}

static int append_codepoint(char *out, size_t cap, size_t *n, uint32_t cp) {
    unsigned char bytes[4];
    size_t count;
    if (cp <= 0x7Fu) { bytes[0] = (unsigned char)cp; count = 1; }
    else if (cp <= 0x7FFu) {
        bytes[0] = (unsigned char)(0xC0u | (cp >> 6));
        bytes[1] = (unsigned char)(0x80u | (cp & 0x3Fu));
        count = 2;
    } else if (cp <= 0xFFFFu && !(cp >= 0xD800u && cp <= 0xDFFFu)) {
        bytes[0] = (unsigned char)(0xE0u | (cp >> 12));
        bytes[1] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
        bytes[2] = (unsigned char)(0x80u | (cp & 0x3Fu));
        count = 3;
    } else if (cp <= 0x10FFFFu) {
        bytes[0] = (unsigned char)(0xF0u | (cp >> 18));
        bytes[1] = (unsigned char)(0x80u | ((cp >> 12) & 0x3Fu));
        bytes[2] = (unsigned char)(0x80u | ((cp >> 6) & 0x3Fu));
        bytes[3] = (unsigned char)(0x80u | (cp & 0x3Fu));
        count = 4;
    } else return 0;
    if (*n + count >= cap) return 0;
    for (size_t j = 0; j < count; ++j) out[(*n)++] = (char)bytes[j];
    return 1;
}

static int decode_entities(char *s, size_t cap) {
    char out[MAX_TEXT];
    size_t i = 0, n = 0;
    while (s[i]) {
        const char *rep = NULL;
        size_t used = 0;
        if (!strncmp(s + i, "&amp;", 5)) { rep = "&"; used = 5; }
        else if (!strncmp(s + i, "&lt;", 4)) { rep = "<"; used = 4; }
        else if (!strncmp(s + i, "&gt;", 4)) { rep = ">"; used = 4; }
        else if (!strncmp(s + i, "&quot;", 6)) { rep = "\""; used = 6; }
        else if (!strncmp(s + i, "&apos;", 6)) { rep = "'"; used = 6; }
        if (rep) {
            if (n + 1 >= sizeof out) return 0;
            out[n++] = *rep;
            i += used;
        } else if (s[i] == '&' && s[i + 1] == '#') {
            size_t j = i + 2;
            unsigned base = 10;
            uint32_t cp = 0;
            unsigned digits = 0;
            if (s[j] == 'x' || s[j] == 'X') { base = 16; ++j; }
            while (s[j] && s[j] != ';') {
                unsigned value;
                if (s[j] >= '0' && s[j] <= '9') value = (unsigned)(s[j] - '0');
                else if (base == 16 && s[j] >= 'a' && s[j] <= 'f') value = 10u + (unsigned)(s[j] - 'a');
                else if (base == 16 && s[j] >= 'A' && s[j] <= 'F') value = 10u + (unsigned)(s[j] - 'A');
                else return 0;
                if (value >= base || cp > (0x10FFFFu - value) / base) return 0;
                cp = cp * base + value;
                ++digits;
                ++j;
            }
            if (!digits || s[j] != ';' || !append_codepoint(out, sizeof out, &n, cp)) return 0;
            i = j + 1;
        } else {
            if (s[i] == '&' || n + 1 >= sizeof out) return 0;
            out[n++] = s[i++];
        }
    }
    out[n] = '\0';
    if (n >= cap) return 0;
    copy_text(s, cap, out);
    return 1;
}

static const char *attr(const Tag *tag, const char *name) {
    for (unsigned i = 0; i < tag->attr_count; ++i)
        if (!strcmp(tag->attrs[i].name, name)) return tag->attrs[i].value;
    return NULL;
}

static int parse_tag(char *raw, Tag *tag) {
    char *p = raw;
    memset(tag, 0, sizeof *tag);
    trim(p);
    if (!*p || *p == '!' || *p == '?') return 2;
    if (*p == '/') { tag->closing = 1; ++p; }
    while (isspace((unsigned char)*p)) ++p;
    size_t n = 0;
    while (*p && !isspace((unsigned char)*p) && *p != '/') {
        if (n + 1 >= sizeof tag->name) return 0;
        tag->name[n++] = *p++;
    }
    tag->name[n] = '\0';
    if (!n) return 0;
    if (tag->closing) return 1;
    while (*p) {
        while (isspace((unsigned char)*p)) ++p;
        if (*p == '/') { tag->self_closing = 1; ++p; continue; }
        if (!*p) break;
        if (tag->attr_count == 16) return 0;
        Attr *a = &tag->attrs[tag->attr_count++];
        n = 0;
        while (*p && !isspace((unsigned char)*p) && *p != '=') {
            if (n + 1 >= sizeof a->name) return 0;
            a->name[n++] = *p++;
        }
        a->name[n] = '\0';
        while (isspace((unsigned char)*p)) ++p;
        if (*p++ != '=') return 0;
        while (isspace((unsigned char)*p)) ++p;
        if (*p != '\'' && *p != '"') return 0;
        char quote = *p++;
        n = 0;
        while (*p && *p != quote) {
            if (n + 1 >= sizeof a->value) return 0;
            a->value[n++] = *p++;
        }
        if (*p++ != quote) return 0;
        a->value[n] = '\0';
        if (!decode_entities(a->value, sizeof a->value)) return 0;
    }
    return 1;
}

static int clean_field(const char *s) {
    return !strchr(s, '|') && !strchr(s, '\n') && !strchr(s, '\r');
}

static void add_row(const char *key, long number, const char *line) {
    if (row_count == MAX_ROWS || !clean_field(key)) {
        ++refused_rows;
        return;
    }
    copy_text(rows[row_count].key, sizeof rows[row_count].key, key);
    copy_text(rows[row_count].line, sizeof rows[row_count].line, line);
    rows[row_count].number = number;
    ++row_count;
}

static void emit_stone(void) {
    char line[MAX_LINE], key[384];
    int written;
    if (!stone.token[0] || !stone.domain[0]) { ++refused_rows; return; }
    if (!clean_field(stone.token) || !clean_field(stone.domain) || !clean_field(stone.say) ||
        !clean_field(stone.word) || !clean_field(stone.examples) || !clean_field(stone.object) ||
        !clean_field(stone.render_ref) || !clean_field(stone.record_ref) || !clean_field(stone.hex) ||
        !clean_field(stone.paint_color) || !clean_field(stone.upper)) {
        ++refused_rows;
        return;
    }
    if (!strcmp(stone.domain, "counting")) {
        char *end = NULL;
        errno = 0;
        long value = strtol(stone.token, &end, 10);
        if (errno || !end || *end || !stone.word[0] || !stone.say[0]) { ++refused_rows; return; }
        written = snprintf(line, sizeof line, "%ld|%s|%s|%s|%s", value, stone.word, stone.say,
                           stone.render_ref, stone.record_ref);
        if (written < 0 || (size_t)written >= sizeof line) { ++refused_rows; return; }
        snprintf(key, sizeof key, "%020ld", value);
        add_row(key, value, line);
    } else if (!strcmp(stone.domain, "alphabet")) {
        if (strlen(stone.token) != 1 || !stone.upper[0] || stone.ordinal < 1 || !stone.say[0]) {
            ++refused_rows; return;
        }
        written = snprintf(line, sizeof line, "%s|%s|%d|%s|%s|%s", stone.token, stone.upper,
                           stone.ordinal, stone.say, stone.render_ref, stone.record_ref);
        if (written < 0 || (size_t)written >= sizeof line) { ++refused_rows; return; }
        add_row(stone.token, stone.ordinal, line);
    } else if (!strcmp(stone.domain, "colors")) {
        const char *status = stone.hex[0] ? "ready" : "unresolved";
        if (!stone.examples[0]) { ++refused_rows; return; }
        if (!stone.hex[0]) ++unresolved_rows;
        written = snprintf(line, sizeof line, "%s|%s|%s|%s|%s|%s", stone.token, stone.hex,
                           stone.examples, status, stone.render_ref, stone.record_ref);
        if (written < 0 || (size_t)written >= sizeof line) { ++refused_rows; return; }
        add_row(stone.token, 0, line);
    } else if (!strcmp(stone.domain, "shapes")) {
        if (stone.sides < 1 || stone.vertices < 1 || stone.edges < 1) { ++refused_rows; return; }
        written = snprintf(line, sizeof line, "%s|%d|%d|%d|%s|%s", stone.token, stone.sides,
                           stone.vertices, stone.edges, stone.render_ref, stone.record_ref);
        if (written < 0 || (size_t)written >= sizeof line) { ++refused_rows; return; }
        add_row(stone.token, stone.sides, line);
    } else if (!strcmp(stone.domain, "color_example")) {
        if (!stone.paint_color[0] || !stone.object[0]) { ++refused_rows; return; }
        snprintf(key, sizeof key, "%s\037%s", stone.paint_color, stone.object);
        written = snprintf(line, sizeof line, "%s|%s", stone.paint_color, stone.object);
        if (written < 0 || (size_t)written >= sizeof line) { ++refused_rows; return; }
        add_row(key, 0, line);
    } else {
        ++refused_rows;
    }
}

static void start_tag(const Tag *tag) {
    const char *v;
    if (!strcmp(tag->name, "ground") && (v = attr(tag, "domain"))) copy_text(root_domain, sizeof root_domain, v);
    if (!strcmp(tag->name, "stone")) {
        memset(&stone, 0, sizeof stone);
        if ((v = attr(tag, "token"))) copy_text(stone.token, sizeof stone.token, v);
        if ((v = attr(tag, "domain"))) copy_text(stone.domain, sizeof stone.domain, v);
    } else if (!strcmp(tag->name, "render") && (v = attr(tag, "ref"))) copy_text(stone.render_ref, sizeof stone.render_ref, v);
    else if (!strcmp(tag->name, "record") && (v = attr(tag, "ref"))) copy_text(stone.record_ref, sizeof stone.record_ref, v);
    else if (!strcmp(tag->name, "swatch")) {
        if ((v = attr(tag, "hex"))) copy_text(stone.hex, sizeof stone.hex, v);
        if ((v = attr(tag, "status"))) copy_text(stone.status, sizeof stone.status, v);
    } else if (!strcmp(tag->name, "paint") && (v = attr(tag, "color"))) copy_text(stone.paint_color, sizeof stone.paint_color, v);
    else if (!strcmp(tag->name, "shape") && (v = attr(tag, "sides"))) stone.sides = atoi(v);
    else if (!strcmp(tag->name, "vertices") && (v = attr(tag, "count"))) stone.vertices = atoi(v);
    else if (!strcmp(tag->name, "edges") && (v = attr(tag, "count"))) stone.edges = atoi(v);
    else if (!strcmp(tag->name, "case")) {
        if ((v = attr(tag, "lower"))) copy_text(stone.lower, sizeof stone.lower, v);
        if ((v = attr(tag, "upper"))) copy_text(stone.upper, sizeof stone.upper, v);
    }
}

static void text_event(char *text) {
    if (!depth) return;
    trim(text);
    if (!*text || !decode_entities(text, MAX_TEXT)) return;
    const char *top = stack_names[depth - 1];
    if (!strcmp(top, "say")) copy_text(stone.say, sizeof stone.say, text);
    else if (!strcmp(top, "word")) copy_text(stone.word, sizeof stone.word, text);
    else if (!strcmp(top, "letters")) copy_text(stone.letters, sizeof stone.letters, text);
    else if (!strcmp(top, "context")) copy_text(stone.context, sizeof stone.context, text);
    else if (!strcmp(top, "examples")) copy_text(stone.examples, sizeof stone.examples, text);
    else if (!strcmp(top, "object")) copy_text(stone.object, sizeof stone.object, text);
    else if (!strcmp(top, "ordinal")) stone.ordinal = atoi(text);
}

static int parse_xml(FILE *fp) {
    char text[MAX_TEXT], raw[MAX_TAG];
    size_t tn = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c != '<') {
            if (tn + 1 >= sizeof text) return 0;
            text[tn++] = (char)c;
            continue;
        }
        text[tn] = '\0';
        text_event(text);
        tn = 0;
        size_t rn = 0;
        int quote = 0;
        c = fgetc(fp);
        if (c == '!') {
            int a = fgetc(fp), b = fgetc(fp);
            if (a != '-' || b != '-') return 0;
            int p2 = 0, p1 = 0;
            while ((c = fgetc(fp)) != EOF) {
                if (p2 == '-' && p1 == '-' && c == '>') break;
                p2 = p1;
                p1 = c;
            }
            if (c == EOF) return 0;
            continue;
        }
        if (c == '?') {
            int previous = 0;
            while ((c = fgetc(fp)) != EOF) {
                if (previous == '?' && c == '>') break;
                previous = c;
            }
            if (c == EOF) return 0;
            continue;
        }
        if (c == EOF || rn + 1 >= sizeof raw) return 0;
        raw[rn++] = (char)c;
        while ((c = fgetc(fp)) != EOF) {
            if (!quote && (c == '\'' || c == '"')) quote = c;
            else if (quote && c == quote) quote = 0;
            else if (!quote && c == '>') break;
            if (rn + 1 >= sizeof raw) return 0;
            raw[rn++] = (char)c;
        }
        if (c == EOF || quote) return 0;
        raw[rn] = '\0';
        Tag tag;
        int parsed = parse_tag(raw, &tag);
        if (!parsed) return 0;
        if (parsed == 2) continue;
        if (tag.closing) {
            if (!depth || strcmp(stack_names[depth - 1], tag.name)) return 0;
            if (!strcmp(tag.name, "stone")) emit_stone();
            --depth;
        } else {
            start_tag(&tag);
            if (!tag.self_closing) {
                if (depth == MAX_DEPTH) return 0;
                copy_text(stack_names[depth++], MAX_NAME, tag.name);
            }
        }
    }
    text[tn] = '\0';
    text_event(text);
    return depth == 0;
}

static int row_cmp(const Row *a, const Row *b) {
    if (!strcmp(root_domain, "counting")) return (a->number > b->number) - (a->number < b->number);
    return strcmp(a->key, b->key);
}

static void sort_rows(void) {
    for (unsigned i = 1; i < row_count; ++i) {
        Row hold = rows[i];
        unsigned j = i;
        while (j && row_cmp(&hold, &rows[j - 1]) < 0) {
            rows[j] = rows[j - 1];
            --j;
        }
        rows[j] = hold;
    }
}

static const char *header_for_domain(void) {
    if (!strcmp(root_domain, "counting")) return "value|word|say|visual_file|audio_file";
    if (!strcmp(root_domain, "alphabet")) return "letter|upper|ordinal|say|visual_file|audio_file";
    if (!strcmp(root_domain, "colors")) return "color|hex|examples|status|visual_file|audio_file";
    if (!strcmp(root_domain, "shapes")) return "shape|sides|vertices|edges|visual_file|audio_file";
    if (!strcmp(root_domain, "color_example")) return "color|object";
    return NULL;
}

int main(int argc, char **argv) {
    FILE *in, *out, *receipt;
    char receipt_path[1024];
    if (argc != 3) {
        fprintf(stderr, "usage: %s OLD_TABLET.xml OUTPUT.tbl\n", argv[0]);
        return 2;
    }
    in = fopen(argv[1], "rb");
    if (!in) { perror(argv[1]); return 3; }
    if (!parse_xml(in)) { fclose(in); fprintf(stderr, "REFUSED malformed XML: %s\n", argv[1]); return 4; }
    fclose(in);
    const char *header = header_for_domain();
    if (!header) { fprintf(stderr, "REFUSED unsupported domain: %s\n", root_domain); return 5; }
    sort_rows();
    for (unsigned i = 1; i < row_count; ++i) {
        if (!strcmp(rows[i - 1].key, rows[i].key)) { ++refused_rows; }
    }
    out = fopen(argv[2], "wb");
    if (!out) { perror(argv[2]); return 6; }
    fprintf(out, "%s\n", header);
    for (unsigned i = 0; i < row_count; ++i) fprintf(out, "%s\n", rows[i].line);
    if (fclose(out)) return 7;
    if (snprintf(receipt_path, sizeof receipt_path, "%s.receipt", argv[2]) >= (int)sizeof receipt_path) return 8;
    receipt = fopen(receipt_path, "wb");
    if (!receipt) return 9;
    fprintf(receipt, "input=%s\ndomain=%s\nrows=%u\nrefused=%u\nunresolved=%u\n",
            argv[1], root_domain, row_count, refused_rows, unresolved_rows);
    fclose(receipt);
    printf("domain=%s rows=%u refused=%u unresolved=%u output=%s\n",
           root_domain, row_count, refused_rows, unresolved_rows, argv[2]);
    return refused_rows ? 10 : 0;
}
