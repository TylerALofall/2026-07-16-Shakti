#ifndef EDEN_LEXICON_H
#define EDEN_LEXICON_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    EDEN_CLASS_NOUN = 0,
    EDEN_CLASS_NOUN_PL,
    EDEN_CLASS_ADJECTIVE,
    EDEN_CLASS_ADVERB,
    EDEN_CLASS_VERB,
    EDEN_CLASS_VERB_T,
    EDEN_CLASS_VERB_I,
    EDEN_CLASS_VERB_FORM,
    EDEN_CLASS_PREPOSITION,
    EDEN_CLASS_PRONOUN,
    EDEN_CLASS_CONJUNCTION,
    EDEN_CLASS_INTERJECTION,
    EDEN_CLASS_UNK,
    EDEN_CLASS_COUNT
} EdenWordClass;

typedef struct {
    const char *form;
    uint8_t class_id;
    const char *infl;
    const char *base;
} EdenLexiconRow;

typedef struct {
    size_t begin;
    size_t end;
} EdenLexiconRange;

const char *eden_class_name(uint8_t class_id);
int eden_class_from_name(const char *name, uint8_t *class_id);
int eden_lexicon_valid(const EdenLexiconRow *rows, size_t count);
int eden_lexicon_find(const EdenLexiconRow *rows, size_t count,
                      const char *form, EdenLexiconRange *range);

#endif
