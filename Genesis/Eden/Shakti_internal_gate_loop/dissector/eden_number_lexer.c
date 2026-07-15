#include "eden_number_lexer.h"

#include <ctype.h>
#include <limits.h>
#include <string.h>

#define MAX_TOKENS 256
#define MAX_TOKEN_TEXT 32

typedef enum { TOKEN_WORD, TOKEN_DIGITS, TOKEN_SYMBOL } TokenKind;

typedef struct {
    TokenKind kind;
    uint16_t start;
    uint16_t end;
    char text[MAX_TOKEN_TEXT];
} Token;

typedef struct {
    const char *word;
    uint8_t value;
} NumberWord;

static const NumberWord number_words[] = {
    {"eight", 8}, {"eighteen", 18}, {"eighty", 80}, {"eleven", 11},
    {"fifteen", 15}, {"fifty", 50}, {"five", 5}, {"forty", 40},
    {"four", 4}, {"fourteen", 14}, {"nine", 9}, {"nineteen", 19},
    {"ninety", 90}, {"one", 1}, {"seven", 7}, {"seventeen", 17},
    {"seventy", 70}, {"six", 6}, {"sixteen", 16}, {"sixty", 60},
    {"ten", 10}, {"thirteen", 13}, {"thirty", 30}, {"three", 3},
    {"twelve", 12}, {"twenty", 20}, {"two", 2}, {"zero", 0}
};

static const char *const math_words[] = {
    "add", "added", "altogether", "difference", "divide", "divided",
    "each", "equal", "equals", "fewer", "less", "minus", "multiplied",
    "multiply", "per", "plus", "product", "quotient", "remain",
    "remaining", "subtract", "sum", "times", "total"
};

static int sorted_strings(const char *const *table, size_t count) {
    for (size_t i = 1; i < count; ++i)
        if (strcmp(table[i - 1], table[i]) >= 0) return 0;
    return 1;
}

int eden_number_tables_valid(void) {
    for (size_t i = 1; i < sizeof number_words / sizeof number_words[0]; ++i)
        if (strcmp(number_words[i - 1].word, number_words[i].word) >= 0) return 0;
    return sorted_strings(math_words, sizeof math_words / sizeof math_words[0]);
}

static int lookup_number_word(const char *word) {
    size_t lo = 0, hi = sizeof number_words / sizeof number_words[0];
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int cmp = strcmp(word, number_words[mid].word);
        if (cmp < 0) hi = mid;
        else if (cmp > 0) lo = mid + 1;
        else return number_words[mid].value;
    }
    return -1;
}

static int lookup_math_word(const char *word) {
    size_t lo = 0, hi = sizeof math_words / sizeof math_words[0];
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int cmp = strcmp(word, math_words[mid]);
        if (cmp < 0) hi = mid;
        else if (cmp > 0) lo = mid + 1;
        else return 1;
    }
    return 0;
}

static int is_magnitude(const char *word) {
    return !strcmp(word, "hundred") || !strcmp(word, "thousand") ||
           !strcmp(word, "million") || !strcmp(word, "billion") ||
           !strcmp(word, "trillion") || !strcmp(word, "point");
}

static int tokenize(const char *text, Token tokens[MAX_TOKENS], size_t *count) {
    size_t i = 0, n = 0, text_len = strlen(text);
    if (text_len > UINT16_MAX) return 0;
    while (i < text_len) {
        if (isspace((unsigned char)text[i])) { ++i; continue; }
        if (n == MAX_TOKENS) return 0;
        Token *t = &tokens[n++];
        t->start = (uint16_t)i;
        size_t k = 0;
        if (isalpha((unsigned char)text[i])) {
            t->kind = TOKEN_WORD;
            while (i < text_len && isalpha((unsigned char)text[i])) {
                if (k + 1 >= sizeof t->text) return 0;
                t->text[k++] = (char)tolower((unsigned char)text[i++]);
            }
        } else if (isdigit((unsigned char)text[i])) {
            t->kind = TOKEN_DIGITS;
            while (i < text_len && isdigit((unsigned char)text[i])) {
                if (k + 1 >= sizeof t->text) return 0;
                t->text[k++] = text[i++];
            }
        } else {
            t->kind = TOKEN_SYMBOL;
            t->text[k++] = text[i++];
        }
        t->text[k] = '\0';
        t->end = (uint16_t)i;
    }
    *count = n;
    return 1;
}

static int parse_digits(const char *s, int64_t *value) {
    uint64_t n = 0;
    for (size_t i = 0; s[i]; ++i) {
        unsigned d = (unsigned)(s[i] - '0');
        if (n > ((uint64_t)INT64_MAX - d) / 10u) return 0;
        n = n * 10u + d;
    }
    *value = (int64_t)n;
    return 1;
}

static int adjacent_with_space(const char *text, const Token *a, const Token *b) {
    for (uint16_t i = a->end; i < b->start; ++i)
        if (!isspace((unsigned char)text[i])) return 0;
    return 1;
}

static int add_number(EdenNumberScan *out, int64_t value, uint16_t start, uint16_t end) {
    if (out->number_count == EDEN_MAX_EXTRACTED_NUMBERS || end < start) return 0;
    EdenNumber *number = &out->numbers[out->number_count++];
    number->value = value;
    number->start = start;
    number->length = (uint16_t)(end - start);
    return 1;
}

const char *eden_route_name(EdenRoute route) {
    switch (route) {
        case EDEN_ROUTE_LANGUAGE: return "LANGUAGE";
        case EDEN_ROUTE_NUMBER_CONTEXT: return "NUMBER_CONTEXT";
        case EDEN_ROUTE_MATH_CANDIDATE: return "MATH_CANDIDATE";
        default: return "REFUSED";
    }
}

int eden_number_scan(const char *text, EdenNumberScan *out) {
    Token tokens[MAX_TOKENS];
    unsigned char used[MAX_TOKENS] = {0};
    size_t token_count = 0;
    if (!text || !out || !eden_number_tables_valid()) return 0;
    memset(out, 0, sizeof *out);
    if (!tokenize(text, tokens, &token_count)) {
        out->route = EDEN_ROUTE_REFUSED;
        return 0;
    }
    for (size_t i = 0; i < token_count; ++i) {
        if (tokens[i].kind == TOKEN_DIGITS) {
            int64_t value;
            if ((i + 2 < token_count && tokens[i + 1].kind == TOKEN_SYMBOL &&
                 (tokens[i + 1].text[0] == '.' || tokens[i + 1].text[0] == ',') &&
                 tokens[i + 2].kind == TOKEN_DIGITS && tokens[i].end == tokens[i + 1].start &&
                 tokens[i + 1].end == tokens[i + 2].start)) {
                out->unresolved_number = 1;
                used[i] = used[i + 1] = used[i + 2] = 1;
                i += 2;
                continue;
            }
            if (!parse_digits(tokens[i].text, &value) ||
                !add_number(out, value, tokens[i].start, tokens[i].end)) {
                out->route = EDEN_ROUTE_REFUSED;
                return 0;
            }
            used[i] = 1;
        } else if (tokens[i].kind == TOKEN_WORD) {
            int first = lookup_number_word(tokens[i].text);
            if (first < 0) continue;
            size_t next = i + 1, end_index = i;
            int value = first;
            if (next < token_count && tokens[next].kind == TOKEN_WORD &&
                adjacent_with_space(text, &tokens[i], &tokens[next]) && is_magnitude(tokens[next].text)) {
                out->unresolved_number = 1;
                used[i] = used[next] = 1;
                ++i;
                continue;
            }
            if (first >= 20 && first % 10 == 0) {
                if (next + 1 < token_count && tokens[next].kind == TOKEN_SYMBOL &&
                    tokens[next].text[0] == '-' && tokens[next].end == tokens[next + 1].start) ++next;
                if (next < token_count && tokens[next].kind == TOKEN_WORD) {
                    int unit = lookup_number_word(tokens[next].text);
                    int joined = next == i + 1 ? adjacent_with_space(text, &tokens[i], &tokens[next]) :
                                                 tokens[i].end == tokens[i + 1].start;
                    if (unit >= 1 && unit <= 9 && joined) {
                        value += unit;
                        end_index = next;
                        used[next] = 1;
                        if (next != i + 1) used[i + 1] = 1;
                    }
                }
            }
            if (end_index + 1 < token_count && tokens[end_index + 1].kind == TOKEN_WORD &&
                adjacent_with_space(text, &tokens[end_index], &tokens[end_index + 1]) &&
                is_magnitude(tokens[end_index + 1].text)) {
                out->unresolved_number = 1;
                used[i] = used[end_index + 1] = 1;
                i = end_index + 1;
                continue;
            }
            if (!add_number(out, value, tokens[i].start, tokens[end_index].end)) {
                out->route = EDEN_ROUTE_REFUSED;
                return 0;
            }
            used[i] = 1;
            i = end_index;
        }
    }
    for (size_t i = 0; i < token_count; ++i) {
        if (used[i]) continue;
        if (tokens[i].kind == TOKEN_WORD && lookup_math_word(tokens[i].text)) out->math_evidence = 1;
        if (tokens[i].kind == TOKEN_SYMBOL && strchr("+*/=<>-", tokens[i].text[0])) out->math_evidence = 1;
        if (i + 1 < token_count && tokens[i].kind == TOKEN_WORD && tokens[i + 1].kind == TOKEN_WORD &&
            !strcmp(tokens[i].text, "how") &&
            (!strcmp(tokens[i + 1].text, "many") || !strcmp(tokens[i + 1].text, "much"))) {
            out->math_evidence = 1;
        }
    }
    if (out->number_count && out->math_evidence) out->route = EDEN_ROUTE_MATH_CANDIDATE;
    else if (out->number_count || out->unresolved_number) out->route = EDEN_ROUTE_NUMBER_CONTEXT;
    else out->route = EDEN_ROUTE_LANGUAGE;
    return 1;
}
