#include "eden_lexicon.h"
#include "eden_lexicon_generated.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    EdenLexiconRange range;
    if (!eden_lexicon_valid(eden_generated_lexicon, EDEN_GENERATED_LEXICON_COUNT)) return 1;
    if (!eden_lexicon_find(eden_generated_lexicon, EDEN_GENERATED_LEXICON_COUNT,
                           "clip", &range)) return 2;
    if (range.end - range.begin != 2) return 3;
    if (strcmp(eden_class_name(eden_generated_lexicon[range.begin].class_id), "noun")) return 4;
    if (strcmp(eden_class_name(eden_generated_lexicon[range.begin + 1].class_id), "verb_form")) return 5;
    if (eden_lexicon_find(eden_generated_lexicon, EDEN_GENERATED_LEXICON_COUNT,
                          "missing", &range)) return 6;
    puts("PASS: exact lexicon range returns every class");
    return 0;
}
