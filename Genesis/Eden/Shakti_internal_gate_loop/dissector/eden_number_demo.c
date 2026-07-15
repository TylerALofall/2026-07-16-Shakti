#include "eden_number_lexer.h"

#include <stdio.h>

int main(int argc, char **argv) {
    EdenNumberScan scan;
    if (argc != 2) {
        fprintf(stderr, "usage: %s \"TEXT\"\n", argv[0]);
        return 2;
    }
    if (!eden_number_scan(argv[1], &scan)) {
        puts("ROUTE REFUSED");
        return 3;
    }
    printf("ROUTE %s numbers=%zu unresolved=%u math_evidence=%u\n",
           eden_route_name(scan.route), scan.number_count,
           (unsigned)scan.unresolved_number, (unsigned)scan.math_evidence);
    for (size_t i = 0; i < scan.number_count; ++i)
        printf("NUM value=%lld start=%u length=%u\n", (long long)scan.numbers[i].value,
               (unsigned)scan.numbers[i].start, (unsigned)scan.numbers[i].length);
    return 0;
}
