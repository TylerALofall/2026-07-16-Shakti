#ifndef EDEN_NUMBER_LEXER_H
#define EDEN_NUMBER_LEXER_H

#include <stddef.h>
#include <stdint.h>

#define EDEN_MAX_EXTRACTED_NUMBERS 64

typedef enum {
    EDEN_ROUTE_LANGUAGE = 0,
    EDEN_ROUTE_NUMBER_CONTEXT = 1,
    EDEN_ROUTE_MATH_CANDIDATE = 2,
    EDEN_ROUTE_REFUSED = 3
} EdenRoute;

typedef struct {
    int64_t value;
    uint16_t start;
    uint16_t length;
} EdenNumber;

typedef struct {
    EdenRoute route;
    uint8_t unresolved_number;
    uint8_t math_evidence;
    size_t number_count;
    EdenNumber numbers[EDEN_MAX_EXTRACTED_NUMBERS];
} EdenNumberScan;

int eden_number_tables_valid(void);
int eden_number_scan(const char *text, EdenNumberScan *out);
const char *eden_route_name(EdenRoute route);

#endif
