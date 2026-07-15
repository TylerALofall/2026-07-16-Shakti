#include "eden_number_lexer.h"

#include <stdio.h>

static int check(const char *text, EdenRoute route, size_t count, int64_t a, int64_t b, int unresolved) {
    EdenNumberScan scan;
    if (!eden_number_scan(text, &scan)) return 0;
    if (scan.route != route || scan.number_count != count || scan.unresolved_number != unresolved) return 0;
    if (count > 0 && scan.numbers[0].value != a) return 0;
    if (count > 1 && scan.numbers[1].value != b) return 0;
    return 1;
}

int main(void) {
    if (!eden_number_tables_valid()) return 1;
    if (!check("What is 2 + 2?", EDEN_ROUTE_MATH_CANDIDATE, 2, 2, 2, 0)) return 2;
    if (!check("How many are twenty-one plus four?", EDEN_ROUTE_MATH_CANDIDATE, 2, 21, 4, 0)) return 3;
    if (!check("I have two sisters.", EDEN_ROUTE_NUMBER_CONTEXT, 1, 2, 0, 0)) return 4;
    if (!check("The blue triangle.", EDEN_ROUTE_LANGUAGE, 0, 0, 0, 0)) return 5;
    if (!check("Ninety-nine red balls minus 3.", EDEN_ROUTE_MATH_CANDIDATE, 2, 99, 3, 0)) return 6;
    if (!check("TWENTY ONE", EDEN_ROUTE_NUMBER_CONTEXT, 1, 21, 0, 0)) return 7;
    if (!check("one hundred", EDEN_ROUTE_NUMBER_CONTEXT, 0, 0, 0, 1)) return 8;
    if (!check("3.5", EDEN_ROUTE_NUMBER_CONTEXT, 0, 0, 0, 1)) return 9;
    if (check("9223372036854775808", EDEN_ROUTE_NUMBER_CONTEXT, 1, 0, 0, 0)) return 10;
    puts("PASS: exact number extraction and three-way routing");
    return 0;
}
