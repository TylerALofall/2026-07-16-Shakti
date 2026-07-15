#include "eden_lexicon.h"

#include <string.h>

static const char *const class_names[EDEN_CLASS_COUNT] = {
    "noun", "noun_pl", "adjective", "adverb", "verb", "verb_t",
    "verb_i", "verb_form", "preposition", "pronoun", "conjunction",
    "interjection", "unk"
};

const char *eden_class_name(uint8_t class_id) {
    return class_id < EDEN_CLASS_COUNT ? class_names[class_id] : NULL;
}

int eden_class_from_name(const char *name, uint8_t *class_id) {
    if (!name || !class_id) return 0;
    for (uint8_t i = 0; i < EDEN_CLASS_COUNT; ++i) {
        if (!strcmp(name, class_names[i])) {
            *class_id = i;
            return 1;
        }
    }
    return 0;
}

static int compare_tail(const EdenLexiconRow *a, const EdenLexiconRow *b) {
    int cmp = strcmp(eden_class_name(a->class_id), eden_class_name(b->class_id));
    if (cmp) return cmp;
    cmp = strcmp(a->infl, b->infl);
    return cmp ? cmp : strcmp(a->base, b->base);
}

int eden_lexicon_valid(const EdenLexiconRow *rows, size_t count) {
    if (!rows && count) return 0;
    for (size_t i = 0; i < count; ++i) {
        if (!rows[i].form || !rows[i].form[0] ||
            !rows[i].infl || !rows[i].base || !rows[i].base[0] ||
            !eden_class_name(rows[i].class_id)) return 0;
        if (i) {
            int cmp = strcmp(rows[i - 1].form, rows[i].form);
            if (cmp > 0 || (!cmp && compare_tail(&rows[i - 1], &rows[i]) >= 0))
                return 0;
        }
    }
    return 1;
}

int eden_lexicon_find(const EdenLexiconRow *rows, size_t count,
                      const char *form, EdenLexiconRange *range) {
    size_t lo = 0, hi = count;
    if (!rows || !form || !range) return 0;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (strcmp(rows[mid].form, form) < 0) lo = mid + 1;
        else hi = mid;
    }
    range->begin = lo;
    hi = count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (strcmp(rows[mid].form, form) <= 0) lo = mid + 1;
        else hi = mid;
    }
    range->end = lo;
    return range->begin < range->end;
}
