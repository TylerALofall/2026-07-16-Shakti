# RELATIONSHIP MASTERS

This directory holds small, themed, source-receipted tables for deterministic
word and lesson relationships. It is not a scraped link dump and it is not a
runtime graph.

## CONTRACT

- One row states one finite classification fact.
- A row never creates an arbitrary pointer between resident stones.
- Lesson graphs are computed by filtering, joining, grouping, ordering, and
  applying exact formulas to class columns.
- Use UTF-8 tab-separated text with one header row and LF line endings.
- Sort admitted rows by `theme_id`, `left_form`, `relation`, `right_form` using
  bytewise order. The build refuses unsorted input.
- Exact word identity is the complete string, never a hash.
- Every non-founder row names an authoritative master and direct locator.
- Only `ADMITTED` rows may be converted into resident C tables.
- `COUNTED` evidence never enters the `ABSOLUTE` region.
- `UNKNOWN` states what is missing. It is not silently omitted or guessed.

## TRUTH KINDS

| Value | Meaning |
|---|---|
| `ABSOLUTE` | exact mathematics, encoding, or formally closed definition |
| `COUNTED` | observed or vocabulary evidence; never absolute mathematics |
| `FOUNDER_DEFINED` | a named project/personal convention supplied by Tyler |
| `UNKNOWN` | missing, ambiguous, contradictory, or not yet sourced |

## REVIEW STATUS

| Value | Meaning |
|---|---|
| `CANDIDATE` | proposed row; excluded from Ground |
| `REVIEWED` | checked but not signed for admission |
| `ADMITTED` | source, scope, spelling, direction, and class are signed |
| `REFUSED` | exact reason is written in `note` |

## RELATION TYPES

Use a precise relation instead of a vague colon or the word `opposite`:

- `case_pair`
- `young_name_for`
- `life_stage_of`
- `role_counterpart`
- `part_of`
- `contains`
- `precedes`
- `converts_to`
- `same_quantity_as`
- `composed_of`
- `located_in`
- `member_of`

Additional relation names require a signed definition before rows use them.
For example, `kitten young_name_for cat` and `king role_counterpart queen` are
different relationships and must not be placed in one undifferentiated matrix.

## FILES

- `TEMPLATE_relationships.tsv` — one-row intake shape for factual relationships.
- `THEME_REGISTRY.tsv` — distinct lesson themes and the computations they permit.
- `ARITHMETIC_DOMAIN_TEMPLATE.tsv` — finite domains that C exhaustively enumerates.
- `TEMPLATE_math_facts.tsv` — exact `A`, `B`, `C`, `D` review shape.

## EXACT MATH ROW

```text
A  B  C  D
```

- `A`: first integer.
- `B`: one operator: `+`, `-`, `*`, `/`, or `%`.
- `C`: second integer.
- `D`: exact result computed by C and retained in the sealed master.

In C, `/` is integer division and `%` is remainder. They are never synonyms.
Division and remainder refuse `C = 0`. An early exact-division domain admits a
row only when `A % C == 0`; fraction lessons require their own rational schema.

The answer is never deleted to make a quiz. A teaching view displays `D`; an
answering view hides `D`, receives Shakti's answer, compares it with the same
sealed `D`, and then may reveal it. Thus shown and hidden lessons cannot drift.

Do not hand-author an exhaustive table. Declare bounds and an acceptance rule in
`ARITHMETIC_DOMAIN_TEMPLATE.tsv`; fixed nested loops enumerate every accepted
`A,B,C` combination and compute `D`. Sort emitted facts by operator enum, `A`,
then `C`.

The spreadsheet review copy is for the founder. Resident tables are produced
from reviewed plain TSV at build time; runtime never parses an `.xlsx` file.
