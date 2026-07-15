# Exact resident lexicon

This C17 component freezes a sorted `form|class|infl|base` matrix into a
`static const` C table at build time. Runtime lookup uses lower and upper binary
bounds and returns every exact row for a form. It never selects one class by
score.

There is no runtime file loading, allocation, COM, daemon, hash identity,
embedding, or probability. The converter also uses fixed buffers and refuses an
empty, oversized, malformed, unsorted, duplicated, or unknown-class row.

The 13 accepted classes are the established master list: noun, noun_pl,
adjective, adverb, verb, verb_t, verb_i, verb_form, preposition, pronoun,
conjunction, interjection, and unk.

This is the classification side of relationship computation. It does not store
word-to-word links and it does not turn counted vocabulary evidence into an
absolute arithmetic fact.

Build and test on macOS/Linux with `sh test_c.sh`. Build on Windows with Clang
using `build_c.bat`; then run the printed `test_lexicon.exe` path.
