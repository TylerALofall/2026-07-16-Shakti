#include "eden_lexicon.h"

#include <stdio.h>
#include <string.h>

#define LINE_CAP 4096

static void emit_string(const char *s) {
    putchar('"');
    for (; *s; ++s) {
        unsigned char c = (unsigned char)*s;
        if (c == '\\' || c == '"') printf("\\%c", c);
        else if (c >= 32 && c <= 126) putchar(c);
        else printf("\\%03o", c);
    }
    putchar('"');
}

static int split_row(char *line, char **form, char **class_name,
                     char **infl, char **base) {
    char *fields[4];
    size_t n = 1;
    fields[0] = line;
    for (char *p = line; *p; ++p) {
        if (*p == '|') {
            if (n == 4) return 0;
            *p = '\0';
            fields[n++] = p + 1;
        }
    }
    if (n != 4 || !fields[0][0] || !fields[1][0] || !fields[3][0]) return 0;
    *form = fields[0];
    *class_name = fields[1];
    *infl = fields[2];
    *base = fields[3];
    return 1;
}

static int compare_fields(const char *a_form, const char *a_class,
                          const char *a_infl, const char *a_base,
                          const char *b_form, const char *b_class,
                          const char *b_infl, const char *b_base) {
    int cmp = strcmp(a_form, b_form);
    if (cmp) return cmp;
    cmp = strcmp(a_class, b_class);
    if (cmp) return cmp;
    cmp = strcmp(a_infl, b_infl);
    return cmp ? cmp : strcmp(a_base, b_base);
}

int main(int argc, char **argv) {
    char line[LINE_CAP];
    char prev_form[LINE_CAP] = "", prev_class[LINE_CAP] = "";
    char prev_infl[LINE_CAP] = "", prev_base[LINE_CAP] = "";
    size_t row = 0;
    FILE *in;
    if (argc != 2) {
        fprintf(stderr, "usage: %s ground_matrix.txt\n", argv[0]);
        return 2;
    }
    in = fopen(argv[1], "rb");
    if (!in) {
        perror(argv[1]);
        return 2;
    }
    puts("#ifndef EDEN_LEXICON_GENERATED_H");
    puts("#define EDEN_LEXICON_GENERATED_H");
    puts("#include \"eden_lexicon.h\"");
    puts("static const EdenLexiconRow eden_generated_lexicon[] = {");
    while (fgets(line, sizeof line, in)) {
        char *form, *class_name, *infl, *base;
        uint8_t class_id;
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[--len] = '\0';
        else if (!feof(in)) {
            fprintf(stderr, "REFUSED row %zu: exceeds %u bytes\n", row + 1,
                    (unsigned)(LINE_CAP - 2));
            fclose(in);
            return 3;
        }
        if (len && line[len - 1] == '\r') line[--len] = '\0';
        if (!len) {
            fprintf(stderr, "REFUSED row %zu: empty row\n", row + 1);
            fclose(in);
            return 3;
        }
        if (!split_row(line, &form, &class_name, &infl, &base) ||
            !eden_class_from_name(class_name, &class_id)) {
            fprintf(stderr, "REFUSED row %zu: expected form|class|infl|base with a known class\n",
                    row + 1);
            fclose(in);
            return 3;
        }
        if (row && compare_fields(prev_form, prev_class, prev_infl, prev_base,
                                  form, class_name, infl, base) >= 0) {
            fprintf(stderr, "REFUSED row %zu: table is unsorted or duplicated\n", row + 1);
            fclose(in);
            return 3;
        }
        fputs("    {", stdout); emit_string(form); printf(", %u, ", (unsigned)class_id);
        emit_string(infl); fputs(", ", stdout); emit_string(base); puts("},");
        strcpy(prev_form, form);
        strcpy(prev_class, class_name);
        strcpy(prev_infl, infl);
        strcpy(prev_base, base);
        ++row;
    }
    if (ferror(in) || fclose(in)) {
        fputs("REFUSED: matrix read failed\n", stderr);
        return 3;
    }
    if (!row) {
        fputs("REFUSED: matrix is empty\n", stderr);
        return 3;
    }
    puts("};");
    printf("#define EDEN_GENERATED_LEXICON_COUNT %zuu\n", row);
    puts("#endif");
    return 0;
}
