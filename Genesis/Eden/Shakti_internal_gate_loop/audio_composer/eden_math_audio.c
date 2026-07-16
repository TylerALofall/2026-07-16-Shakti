#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define eden_mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define eden_mkdir(path) mkdir((path), 0777)
#endif

/*
 * Eden deterministic math-audio composer.
 *
 * C17, fixed storage, no malloc, no hashes, no probability, no subprocesses.
 * Every output sample comes from a named PCM atom or an explicitly requested
 * count of zero-valued gap samples.
 */

#define EDEN_SAMPLE_RATE 16000u
#define EDEN_CHANNELS 1u
#define EDEN_BITS_PER_SAMPLE 16u
#define EDEN_SIGNAL_FLOOR 200u
#define EDEN_MAX_PATH 1024u
#define EDEN_MAX_PARTS 16u
#define EDEN_COPY_BYTES 8192u
#define EDEN_MAX_GAP_SAMPLES 16000u

_Static_assert(sizeof(uint16_t) == 2u, "uint16_t must be 2 bytes");
_Static_assert(sizeof(uint32_t) == 4u, "uint32_t must be 4 bytes");

typedef enum {
    PART_COUNTING = 1,
    PART_WORD = 2
} PartKind;

typedef struct {
    PartKind kind;
    const char *word;
} Part;

typedef struct {
    Part part[EDEN_MAX_PARTS];
    size_t count;
} Phrase;

typedef struct {
    uint32_t data_offset;
    uint32_t data_bytes;
    uint32_t samples_above_floor;
    uint32_t max_abs;
} WavInfo;

typedef enum {
    OP_ADD = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_DIV = 3
} Operation;

typedef enum {
    MODE_QUESTIONS = 1,
    MODE_ANSWERS = 2,
    MODE_ALL = 3
} BuildMode;

static const char *const SMALL_NUMBER[20] = {
    "zero", "one", "two", "three", "four", "five", "six", "seven",
    "eight", "nine", "ten", "eleven", "twelve", "thirteen",
    "fourteen", "fifteen", "sixteen", "seventeen", "eighteen",
    "nineteen"
};

static const char *const TENS_NUMBER[10] = {
    NULL, NULL, "twenty", "thirty", "forty", "fifty", "sixty",
    "seventy", "eighty", "ninety"
};

static const char *const OP_LABEL[4] = {"Add", "Sub", "Mul", "Div"};
static const char *const OP_WORD[4] = {"plus", "minus", "times", "divide"};

static const char *const COLOR_WORD[7] = {
    "blue", "brown", "green", "orange", "purple", "red", "yellow"
};

static uint16_t read_le16(const unsigned char *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_le32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void write_le16(unsigned char *p, uint16_t value) {
    p[0] = (unsigned char)(value & 0xffu);
    p[1] = (unsigned char)((value >> 8) & 0xffu);
}

static void write_le32(unsigned char *p, uint32_t value) {
    p[0] = (unsigned char)(value & 0xffu);
    p[1] = (unsigned char)((value >> 8) & 0xffu);
    p[2] = (unsigned char)((value >> 16) & 0xffu);
    p[3] = (unsigned char)((value >> 24) & 0xffu);
}

static int path_join(char *dst, size_t cap, const char *left,
                     const char *right) {
    size_t n = strlen(left);
    const char *separator = (n > 0u && (left[n - 1u] == '/' || left[n - 1u] == '\\'))
                                ? ""
                                : "/";
    int written = snprintf(dst, cap, "%s%s%s", left, separator, right);
    if (written < 0 || (size_t)written >= cap) {
        fprintf(stderr, "REFUSED path exceeds %u bytes: %s / %s\n",
                EDEN_MAX_PATH - 1u, left, right);
        return 0;
    }
    return 1;
}

static int atom_path(char *dst, size_t cap, const char *root,
                     const Part *part) {
    char nested_relative[256];
    char flat_relative[256];
    char nested[EDEN_MAX_PATH];
    char flat[EDEN_MAX_PATH];
    FILE *nested_fp;
    FILE *flat_fp;
    int have_nested;
    int have_flat;
    int nested_written;
    int flat_written;
    if (part->kind == PART_COUNTING) {
        nested_written = snprintf(nested_relative, sizeof nested_relative,
                                  "counting/lvl1_counting_%s.wav", part->word);
        flat_written = snprintf(flat_relative, sizeof flat_relative,
                                "lvl1_counting_%s.wav", part->word);
    } else {
        nested_written = snprintf(nested_relative, sizeof nested_relative,
                                  "UNFILED_no_level_yet/%s.wav", part->word);
        flat_written = snprintf(flat_relative, sizeof flat_relative,
                                "%s.wav", part->word);
    }
    if (nested_written < 0 || (size_t)nested_written >= sizeof nested_relative ||
        flat_written < 0 || (size_t)flat_written >= sizeof flat_relative ||
        !path_join(nested, sizeof nested, root, nested_relative) ||
        !path_join(flat, sizeof flat, root, flat_relative)) {
        fprintf(stderr, "REFUSED atom name is too long: %s\n", part->word);
        return 0;
    }
    nested_fp = fopen(nested, "rb");
    flat_fp = fopen(flat, "rb");
    have_nested = nested_fp != NULL;
    have_flat = flat_fp != NULL;
    if (nested_fp != NULL) {
        fclose(nested_fp);
    }
    if (flat_fp != NULL) {
        fclose(flat_fp);
    }
    if (have_nested && have_flat) {
        fprintf(stderr, "REFUSED ambiguous atom exists at both paths: %s AND %s\n",
                nested, flat);
        return 0;
    }
    if (!have_nested && !have_flat) {
        fprintf(stderr, "REFUSED missing WAV atom; accepted paths: %s OR %s\n",
                nested, flat);
        return 0;
    }
    {
        const char *chosen = have_nested ? nested : flat;
        int written = snprintf(dst, cap, "%s", chosen);
        if (written < 0 || (size_t)written >= cap) {
            fprintf(stderr, "REFUSED resolved atom path exceeds fixed buffer: %s\n",
                    chosen);
            return 0;
        }
    }
    return 1;
}

static int phrase_add(Phrase *phrase, PartKind kind, const char *word) {
    if (phrase->count >= EDEN_MAX_PARTS) {
        fputs("REFUSED phrase exceeds fixed part capacity\n", stderr);
        return 0;
    }
    phrase->part[phrase->count].kind = kind;
    phrase->part[phrase->count].word = word;
    ++phrase->count;
    return 1;
}

static int print_phrase_trace(FILE *stream, const Phrase *phrase) {
    size_t i;
    for (i = 0u; i < phrase->count; ++i) {
        if (fprintf(stream, "%s%s:%s", i == 0u ? "" : "|",
                    phrase->part[i].kind == PART_COUNTING ? "counting" : "word",
                    phrase->part[i].word) < 0) {
            return 0;
        }
    }
    return 1;
}

static int phrase_add_number_under_100(Phrase *phrase, unsigned value) {
    unsigned tens;
    unsigned ones;
    if (value <= 19u) {
        return phrase_add(phrase, PART_COUNTING, SMALL_NUMBER[value]);
    }
    tens = value / 10u;
    ones = value % 10u;
    if (tens < 2u || tens > 9u || !phrase_add(phrase, PART_COUNTING,
                                               TENS_NUMBER[tens])) {
        return 0;
    }
    return ones == 0u || phrase_add(phrase, PART_COUNTING, SMALL_NUMBER[ones]);
}

static int phrase_add_number(Phrase *phrase, int value) {
    unsigned magnitude;
    unsigned hundreds;
    unsigned remainder;
    if (value < -400 || value > 400) {
        fprintf(stderr, "REFUSED number outside closed range -400..400: %d\n",
                value);
        return 0;
    }
    if (value < 0) {
        if (!phrase_add(phrase, PART_WORD, "minus")) {
            return 0;
        }
        magnitude = (unsigned)(-value);
    } else {
        magnitude = (unsigned)value;
    }
    if (magnitude < 100u) {
        return phrase_add_number_under_100(phrase, magnitude);
    }
    hundreds = magnitude / 100u;
    remainder = magnitude % 100u;
    if (!phrase_add(phrase, PART_COUNTING, SMALL_NUMBER[hundreds]) ||
        !phrase_add(phrase, PART_COUNTING, "hundred")) {
        return 0;
    }
    if (remainder == 0u) {
        return 1;
    }
    return phrase_add(phrase, PART_COUNTING, "and") &&
           phrase_add_number_under_100(phrase, remainder);
}

static int parse_operation(const char *text, Operation *operation) {
    size_t i;
    for (i = 0u; i < 4u; ++i) {
        if (strcmp(text, OP_LABEL[i]) == 0) {
            *operation = (Operation)i;
            return 1;
        }
    }
    fprintf(stderr, "REFUSED operation must be Add, Sub, Mul, or Div: %s\n", text);
    return 0;
}

static int operation_answer(Operation operation, int a, int c, int *answer) {
    if (operation != OP_DIV) {
        if (a < 1 || a > 20 || c < 1 || c > 20) {
            fputs("REFUSED Add/Sub/Mul operands must both be 1..20\n", stderr);
            return 0;
        }
    } else if (c < 1 || c > 20 || a < 1 || a > 400 || a % c != 0 ||
               a / c < 1 || a / c > 20) {
        fputs("REFUSED Div must use divisor 1..20 and integer quotient 1..20\n",
              stderr);
        return 0;
    }
    switch (operation) {
        case OP_ADD: *answer = a + c; break;
        case OP_SUB: *answer = a - c; break;
        case OP_MUL: *answer = a * c; break;
        case OP_DIV: *answer = a / c; break;
        default: return 0;
    }
    return 1;
}

static int build_math_phrase(Phrase *phrase, int include_answer,
                             Operation operation, int a, int c, int answer) {
    phrase->count = 0u;
    if (!phrase_add_number(phrase, a) ||
        !phrase_add(phrase, PART_WORD, OP_WORD[operation]) ||
        !phrase_add_number(phrase, c) ||
        !phrase_add(phrase, PART_WORD, "equals")) {
        return 0;
    }
    return !include_answer || phrase_add_number(phrase, answer);
}

static int parse_wav(const char *path, WavInfo *info, int require_signal) {
    unsigned char riff[12];
    unsigned char chunk[8];
    unsigned char fmt[64];
    unsigned char samples[EDEN_COPY_BYTES];
    uint16_t format = 0u;
    uint16_t channels = 0u;
    uint16_t block_align = 0u;
    uint16_t bits = 0u;
    uint32_t sample_rate = 0u;
    uint32_t byte_rate = 0u;
    uint32_t data_offset = 0u;
    uint32_t data_bytes = 0u;
    int have_data = 0;
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "REFUSED missing WAV atom: %s\n", path);
        return 0;
    }
    if (fread(riff, 1u, sizeof riff, fp) != sizeof riff ||
        memcmp(riff, "RIFF", 4u) != 0 || memcmp(riff + 8u, "WAVE", 4u) != 0) {
        fprintf(stderr, "REFUSED not a RIFF/WAVE file: %s\n", path);
        fclose(fp);
        return 0;
    }
    while (fread(chunk, 1u, sizeof chunk, fp) == sizeof chunk) {
        uint32_t length = read_le32(chunk + 4u);
        long body = ftell(fp);
        uint64_t next;
        if (body < 0) {
            fprintf(stderr, "REFUSED cannot locate WAV chunk: %s\n", path);
            fclose(fp);
            return 0;
        }
        next = (uint64_t)(unsigned long)body + (uint64_t)length +
               (uint64_t)(length & 1u);
        if (next > (uint64_t)LONG_MAX) {
            fprintf(stderr, "REFUSED WAV chunk offset is too large: %s\n", path);
            fclose(fp);
            return 0;
        }
        if (memcmp(chunk, "fmt ", 4u) == 0) {
            size_t take = length < sizeof fmt ? (size_t)length : sizeof fmt;
            if (take < 16u || fread(fmt, 1u, take, fp) != take) {
                fprintf(stderr, "REFUSED malformed WAV fmt chunk: %s\n", path);
                fclose(fp);
                return 0;
            }
            format = read_le16(fmt);
            channels = read_le16(fmt + 2u);
            sample_rate = read_le32(fmt + 4u);
            byte_rate = read_le32(fmt + 8u);
            block_align = read_le16(fmt + 12u);
            bits = read_le16(fmt + 14u);
        } else if (memcmp(chunk, "data", 4u) == 0) {
            if ((uint64_t)(unsigned long)body > UINT32_MAX) {
                fprintf(stderr, "REFUSED WAV data offset is too large: %s\n", path);
                fclose(fp);
                return 0;
            }
            data_offset = (uint32_t)body;
            data_bytes = length;
            have_data = 1;
        }
        if (fseek(fp, (long)next, SEEK_SET) != 0) {
            fprintf(stderr, "REFUSED malformed WAV chunk length: %s\n", path);
            fclose(fp);
            return 0;
        }
    }
    if (format != 1u || channels != EDEN_CHANNELS ||
        sample_rate != EDEN_SAMPLE_RATE || bits != EDEN_BITS_PER_SAMPLE ||
        block_align != 2u || byte_rate != 32000u || !have_data ||
        (data_bytes & 1u) != 0u) {
        fprintf(stderr,
                "REFUSED WAV must be PCM mono 16-bit 16000 Hz: %s"
                " (format=%u channels=%u rate=%u bits=%u)\n",
                path, format, channels, sample_rate, bits);
        fclose(fp);
        return 0;
    }
    info->data_offset = data_offset;
    info->data_bytes = data_bytes;
    info->samples_above_floor = 0u;
    info->max_abs = 0u;
    if (require_signal) {
        uint32_t remaining = data_bytes;
        if (fseek(fp, (long)data_offset, SEEK_SET) != 0) {
            fprintf(stderr, "REFUSED cannot seek WAV data: %s\n", path);
            fclose(fp);
            return 0;
        }
        while (remaining > 0u) {
            size_t want = remaining < sizeof samples ? (size_t)remaining
                                                     : sizeof samples;
            size_t got = fread(samples, 1u, want, fp);
            size_t i;
            if (got != want) {
                fprintf(stderr, "REFUSED truncated WAV data: %s\n", path);
                fclose(fp);
                return 0;
            }
            for (i = 0u; i + 1u < got; i += 2u) {
                int32_t sample = (int16_t)read_le16(samples + i);
                uint32_t absolute = (uint32_t)(sample < 0 ? -sample : sample);
                if (absolute > info->max_abs) {
                    info->max_abs = absolute;
                }
                if (absolute > EDEN_SIGNAL_FLOOR) {
                    ++info->samples_above_floor;
                }
            }
            remaining -= (uint32_t)got;
        }
        if (info->samples_above_floor == 0u) {
            fprintf(stderr, "REFUSED silent WAV atom: %s\n", path);
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1;
}

static int preflight_part(const char *root, PartKind kind, const char *word) {
    Part part;
    char path[EDEN_MAX_PATH];
    WavInfo info;
    part.kind = kind;
    part.word = word;
    return atom_path(path, sizeof path, root, &part) &&
           parse_wav(path, &info, 1);
}

static int preflight_atoms(const char *root, int require_zero,
                           int require_colors) {
    unsigned i;
    static const char *const other_counting[] = {
        "hundred", "and"
    };
    static const char *const math_words[] = {
        "plus", "minus", "times", "divide", "equals"
    };
    for (i = 1u; i <= 19u; ++i) {
        if (!preflight_part(root, PART_COUNTING, SMALL_NUMBER[i])) {
            return 0;
        }
    }
    for (i = 2u; i <= 9u; ++i) {
        if (!preflight_part(root, PART_COUNTING, TENS_NUMBER[i])) {
            return 0;
        }
    }
    for (i = 0u; i < sizeof other_counting / sizeof other_counting[0]; ++i) {
        if (!preflight_part(root, PART_COUNTING, other_counting[i])) {
            return 0;
        }
    }
    for (i = 0u; i < sizeof math_words / sizeof math_words[0]; ++i) {
        if (!preflight_part(root, PART_WORD, math_words[i])) {
            return 0;
        }
    }
    if (require_zero && !preflight_part(root, PART_COUNTING, "zero")) {
        return 0;
    }
    if (require_colors) {
        for (i = 0u; i < sizeof COLOR_WORD / sizeof COLOR_WORD[0]; ++i) {
            if (!preflight_part(root, PART_WORD, COLOR_WORD[i])) {
                return 0;
            }
        }
        if (!preflight_part(root, PART_WORD, "balls")) {
            return 0;
        }
    }
    return 1;
}

static int write_wav_header(FILE *fp, uint32_t data_bytes) {
    unsigned char header[44] = {0};
    memcpy(header, "RIFF", 4u);
    write_le32(header + 4u, 36u + data_bytes);
    memcpy(header + 8u, "WAVEfmt ", 8u);
    write_le32(header + 16u, 16u);
    write_le16(header + 20u, 1u);
    write_le16(header + 22u, EDEN_CHANNELS);
    write_le32(header + 24u, EDEN_SAMPLE_RATE);
    write_le32(header + 28u, 32000u);
    write_le16(header + 32u, 2u);
    write_le16(header + 34u, EDEN_BITS_PER_SAMPLE);
    memcpy(header + 36u, "data", 4u);
    write_le32(header + 40u, data_bytes);
    return fwrite(header, 1u, sizeof header, fp) == sizeof header;
}

static int output_does_not_exist(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return 1;
    }
    fclose(fp);
    fprintf(stderr, "REFUSED output already exists: %s\n", path);
    return 0;
}

static int write_zero_samples(FILE *fp, uint32_t samples) {
    static const unsigned char zero[512] = {0};
    uint32_t bytes = samples * 2u;
    while (bytes > 0u) {
        size_t take = bytes < sizeof zero ? (size_t)bytes : sizeof zero;
        if (fwrite(zero, 1u, take, fp) != take) {
            return 0;
        }
        bytes -= (uint32_t)take;
    }
    return 1;
}

static int copy_atom_data(FILE *out, const char *path, const WavInfo *info) {
    unsigned char buffer[EDEN_COPY_BYTES];
    uint32_t remaining = info->data_bytes;
    FILE *in = fopen(path, "rb");
    if (in == NULL || fseek(in, (long)info->data_offset, SEEK_SET) != 0) {
        fprintf(stderr, "REFUSED cannot reopen WAV atom: %s\n", path);
        if (in != NULL) {
            fclose(in);
        }
        return 0;
    }
    while (remaining > 0u) {
        size_t take = remaining < sizeof buffer ? (size_t)remaining
                                                : sizeof buffer;
        if (fread(buffer, 1u, take, in) != take ||
            fwrite(buffer, 1u, take, out) != take) {
            fprintf(stderr, "REFUSED failed while copying WAV atom: %s\n", path);
            fclose(in);
            return 0;
        }
        remaining -= (uint32_t)take;
    }
    fclose(in);
    return 1;
}

static int compose_phrase(const char *root, const char *output,
                          const Phrase *phrase, uint32_t gap_samples,
                          uint32_t *written_samples) {
    WavInfo info[EDEN_MAX_PARTS];
    char paths[EDEN_MAX_PARTS][EDEN_MAX_PATH];
    char temporary[EDEN_MAX_PATH];
    uint64_t total_bytes = 0u;
    size_t i;
    FILE *out;
    if (phrase->count == 0u || phrase->count > EDEN_MAX_PARTS ||
        !output_does_not_exist(output)) {
        return 0;
    }
    for (i = 0u; i < phrase->count; ++i) {
        if (!atom_path(paths[i], sizeof paths[i], root, &phrase->part[i]) ||
            !parse_wav(paths[i], &info[i], 0)) {
            return 0;
        }
        total_bytes += info[i].data_bytes;
    }
    if (phrase->count > 1u) {
        total_bytes += (uint64_t)(phrase->count - 1u) * gap_samples * 2u;
    }
    if (total_bytes > UINT32_MAX - 36u) {
        fputs("REFUSED composed WAV would exceed RIFF 32-bit size\n", stderr);
        return 0;
    }
    if (snprintf(temporary, sizeof temporary, "%s.tmp", output) < 0 ||
        strlen(output) + 4u >= sizeof temporary ||
        !output_does_not_exist(temporary)) {
        return 0;
    }
    out = fopen(temporary, "wb");
    if (out == NULL) {
        fprintf(stderr, "REFUSED cannot create temporary output: %s\n", temporary);
        return 0;
    }
    if (!write_wav_header(out, (uint32_t)total_bytes)) {
        fprintf(stderr, "REFUSED cannot write WAV header: %s\n", temporary);
        fclose(out);
        remove(temporary);
        return 0;
    }
    for (i = 0u; i < phrase->count; ++i) {
        if (!copy_atom_data(out, paths[i], &info[i]) ||
            (i + 1u < phrase->count && !write_zero_samples(out, gap_samples))) {
            fclose(out);
            remove(temporary);
            return 0;
        }
    }
    if (fclose(out) != 0 || rename(temporary, output) != 0) {
        fprintf(stderr, "REFUSED cannot finalize output: %s\n", output);
        remove(temporary);
        return 0;
    }
    *written_samples = (uint32_t)(total_bytes / 2u);
    return 1;
}

static int ensure_directory(const char *path) {
    if (eden_mkdir(path) == 0 || errno == EEXIST) {
        return 1;
    }
    fprintf(stderr, "REFUSED cannot create directory: %s\n", path);
    return 0;
}

static int parse_unsigned(const char *text, uint32_t maximum, uint32_t *value) {
    char *end = NULL;
    unsigned long parsed;
    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed > maximum) {
        fprintf(stderr, "REFUSED invalid unsigned number 0..%u: %s\n", maximum,
                text);
        return 0;
    }
    *value = (uint32_t)parsed;
    return 1;
}

static int parse_int(const char *text, int minimum, int maximum, int *value) {
    char *end = NULL;
    long parsed;
    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed < minimum ||
        parsed > maximum) {
        fprintf(stderr, "REFUSED invalid integer %d..%d: %s\n", minimum,
                maximum, text);
        return 0;
    }
    *value = (int)parsed;
    return 1;
}

static int parse_mode(const char *text, BuildMode *mode) {
    if (strcmp(text, "questions") == 0) {
        *mode = MODE_QUESTIONS;
    } else if (strcmp(text, "answers") == 0) {
        *mode = MODE_ANSWERS;
    } else if (strcmp(text, "all") == 0) {
        *mode = MODE_ALL;
    } else {
        fprintf(stderr, "REFUSED mode must be questions, answers, or all: %s\n",
                text);
        return 0;
    }
    return 1;
}

static int make_card_path(char *path, size_t cap, const char *directory,
                          Operation operation, int a, int c) {
    char name[96];
    int written = snprintf(name, sizeof name, "math_%s_%d-%d.wav",
                           OP_LABEL[operation], a, c);
    return written >= 0 && (size_t)written < sizeof name &&
           path_join(path, cap, directory, name);
}

static int preflight_output_set(const char *directory) {
    Operation operation;
    char path[EDEN_MAX_PATH];
    for (operation = OP_ADD; operation <= OP_DIV;
         operation = (Operation)(operation + 1)) {
        int a;
        int c;
        if (operation == OP_DIV) {
            int quotient;
            for (c = 1; c <= 20; ++c) {
                for (quotient = 1; quotient <= 20; ++quotient) {
                    a = c * quotient;
                    if (!make_card_path(path, sizeof path, directory, operation,
                                        a, c) || !output_does_not_exist(path)) {
                        return 0;
                    }
                }
            }
        } else {
            for (a = 1; a <= 20; ++a) {
                for (c = 1; c <= 20; ++c) {
                    if (!make_card_path(path, sizeof path, directory, operation,
                                        a, c) || !output_does_not_exist(path)) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

static int generate_card(FILE *receipt, const char *root, const char *directory,
                         int include_answer, Operation operation, int a, int c,
                         uint32_t gap_samples) {
    Phrase phrase;
    char path[EDEN_MAX_PATH];
    int answer;
    uint32_t samples;
    if (!operation_answer(operation, a, c, &answer) ||
        !build_math_phrase(&phrase, include_answer, operation, a, c, answer) ||
        !make_card_path(path, sizeof path, directory, operation, a, c) ||
        !compose_phrase(root, path, &phrase, gap_samples, &samples)) {
        return 0;
    }
    if (fprintf(receipt, "math_%s_%d-%d.wav\t%d\t%s\t%d\t%d\t%u\t%u\t",
                OP_LABEL[operation], a, c, a, OP_LABEL[operation], c, answer,
                samples, EDEN_SAMPLE_RATE) < 0) {
        fprintf(stderr, "REFUSED cannot write receipt for: %s\n", path);
        return 0;
    }
    {
        if (!print_phrase_trace(receipt, &phrase) || fputc('\n', receipt) == EOF) {
            fprintf(stderr, "REFUSED cannot finish receipt row for: %s\n", path);
            return 0;
        }
    }
    return 1;
}

static int generate_set(const char *root, const char *directory,
                        int include_answer, uint32_t gap_samples) {
    char receipt_path[EDEN_MAX_PATH];
    FILE *receipt;
    unsigned count = 0u;
    Operation operation;
    if (!path_join(receipt_path, sizeof receipt_path, directory,
                   "MATH_AUDIO_MANIFEST.tsv") ||
        !output_does_not_exist(receipt_path)) {
        return 0;
    }
    receipt = fopen(receipt_path, "wb");
    if (receipt == NULL) {
        fprintf(stderr, "REFUSED cannot create manifest: %s\n", receipt_path);
        return 0;
    }
    fputs("file_path\tA\toperation\tC\tanswer\tsamples\tsample_rate\tspoken_atoms\n",
          receipt);
    for (operation = OP_ADD; operation <= OP_DIV;
         operation = (Operation)(operation + 1)) {
        int a;
        int c;
        if (operation == OP_DIV) {
            int quotient;
            for (c = 1; c <= 20; ++c) {
                for (quotient = 1; quotient <= 20; ++quotient) {
                    a = c * quotient;
                    if (!generate_card(receipt, root, directory, include_answer,
                                       operation, a, c, gap_samples)) {
                        fclose(receipt);
                        return 0;
                    }
                    ++count;
                }
            }
        } else {
            for (a = 1; a <= 20; ++a) {
                for (c = 1; c <= 20; ++c) {
                    if (!generate_card(receipt, root, directory, include_answer,
                                       operation, a, c, gap_samples)) {
                        fclose(receipt);
                        return 0;
                    }
                    ++count;
                }
            }
        }
    }
    if (fclose(receipt) != 0 || count != 1600u) {
        fprintf(stderr, "REFUSED incomplete output set: %u of 1600\n", count);
        return 0;
    }
    printf("PASS wrote %u %s WAV files and %s\n", count,
           include_answer ? "answer" : "question", receipt_path);
    return 1;
}

static int build_all(const char *root, const char *output_root, BuildMode mode,
                     uint32_t gap_samples) {
    char question_dir[EDEN_MAX_PATH];
    char answer_dir[EDEN_MAX_PATH];
    int need_questions = mode == MODE_QUESTIONS || mode == MODE_ALL;
    int need_answers = mode == MODE_ANSWERS || mode == MODE_ALL;
    if (!preflight_atoms(root, need_answers, 0)) {
        return 0;
    }
    if (!ensure_directory(output_root) ||
        !path_join(question_dir, sizeof question_dir, output_root, "questions") ||
        !path_join(answer_dir, sizeof answer_dir, output_root, "answers")) {
        return 0;
    }
    if (need_questions && !ensure_directory(question_dir)) {
        return 0;
    }
    if (need_answers && !ensure_directory(answer_dir)) {
        return 0;
    }
    if ((need_questions && !preflight_output_set(question_dir)) ||
        (need_answers && !preflight_output_set(answer_dir))) {
        return 0;
    }
    if (need_questions) {
        char manifest[EDEN_MAX_PATH];
        if (!path_join(manifest, sizeof manifest, question_dir,
                       "MATH_AUDIO_MANIFEST.tsv") ||
            !output_does_not_exist(manifest)) {
            return 0;
        }
    }
    if (need_answers) {
        char manifest[EDEN_MAX_PATH];
        if (!path_join(manifest, sizeof manifest, answer_dir,
                       "MATH_AUDIO_MANIFEST.tsv") ||
            !output_does_not_exist(manifest)) {
            return 0;
        }
    }
    return (!need_questions || generate_set(root, question_dir, 0, gap_samples)) &&
           (!need_answers || generate_set(root, answer_dir, 1, gap_samples));
}

static int color_is_known(const char *color) {
    size_t i;
    for (i = 0u; i < sizeof COLOR_WORD / sizeof COLOR_WORD[0]; ++i) {
        if (strcmp(color, COLOR_WORD[i]) == 0) {
            return 1;
        }
    }
    fprintf(stderr,
            "REFUSED color must be blue, brown, green, orange, purple, red, or yellow: %s\n",
            color);
    return 0;
}

static int compose_color_balls(const char *root, const char *output,
                               int has_count, int count, const char *color,
                               uint32_t gap_samples) {
    Phrase phrase;
    uint32_t samples;
    phrase.count = 0u;
    if (!color_is_known(color) || !preflight_atoms(root, has_count && count == 0, 1)) {
        return 0;
    }
    if (has_count && !phrase_add_number(&phrase, count)) {
        return 0;
    }
    if (!phrase_add(&phrase, PART_WORD, color) ||
        !phrase_add(&phrase, PART_WORD, "balls") ||
        !compose_phrase(root, output, &phrase, gap_samples, &samples)) {
        return 0;
    }
    printf("PASS wrote color-balls WAV: %s (%u samples at %u Hz)\nATOMS ",
           output, samples, EDEN_SAMPLE_RATE);
    if (!print_phrase_trace(stdout, &phrase) || fputc('\n', stdout) == EOF) {
        return 0;
    }
    return 1;
}

static void usage(const char *program) {
    fprintf(stderr,
            "Usage:\n"
            "  %s preflight ATOM_ROOT questions|answers|all\n"
            "  %s build ATOM_ROOT OUTPUT_ROOT questions|answers|all [GAP_SAMPLES]\n"
            "  %s single ATOM_ROOT OUTPUT.wav question|answer Add|Sub|Mul|Div A C [GAP_SAMPLES]\n"
            "  %s color-balls ATOM_ROOT OUTPUT.wav COLOR [COUNT] [GAP_SAMPLES]\n"
            "\nGAP_SAMPLES defaults to 0 and may be 0..16000. Output files are never overwritten.\n",
            program, program, program, program);
}

int main(int argc, char **argv) {
    BuildMode mode;
    uint32_t gap_samples = 0u;
    if (argc == 4 && strcmp(argv[1], "preflight") == 0) {
        if (!parse_mode(argv[3], &mode) ||
            !preflight_atoms(argv[2], mode != MODE_QUESTIONS, 0)) {
            return 2;
        }
        printf("PASS required %u Hz PCM atoms have signal for mode %s\n",
               EDEN_SAMPLE_RATE, argv[3]);
        return 0;
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "build") == 0) {
        if (!parse_mode(argv[4], &mode) ||
            (argc == 6 && !parse_unsigned(argv[5], EDEN_MAX_GAP_SAMPLES,
                                          &gap_samples)) ||
            !build_all(argv[2], argv[3], mode, gap_samples)) {
            return 2;
        }
        return 0;
    }
    if ((argc == 8 || argc == 9) && strcmp(argv[1], "single") == 0) {
        Operation operation;
        int a;
        int c;
        int answer;
        int include_answer;
        Phrase phrase;
        uint32_t samples;
        if (strcmp(argv[4], "question") == 0) {
            include_answer = 0;
        } else if (strcmp(argv[4], "answer") == 0) {
            include_answer = 1;
        } else {
            fprintf(stderr, "REFUSED single kind must be question or answer: %s\n",
                    argv[4]);
            return 2;
        }
        if (!parse_operation(argv[5], &operation) ||
            !parse_int(argv[6], 1, 400, &a) ||
            !parse_int(argv[7], 1, 20, &c) ||
            (argc == 9 && !parse_unsigned(argv[8], EDEN_MAX_GAP_SAMPLES,
                                          &gap_samples)) ||
            !operation_answer(operation, a, c, &answer) ||
            !preflight_atoms(argv[2], include_answer && answer == 0, 0) ||
            !build_math_phrase(&phrase, include_answer, operation, a, c, answer) ||
            !compose_phrase(argv[2], argv[3], &phrase, gap_samples, &samples)) {
            return 2;
        }
        printf("PASS wrote %s WAV: %s (%u samples at %u Hz)\nATOMS ", argv[4],
               argv[3], samples, EDEN_SAMPLE_RATE);
        if (!print_phrase_trace(stdout, &phrase) || fputc('\n', stdout) == EOF) {
            return 2;
        }
        return 0;
    }
    if ((argc >= 5 && argc <= 7) && strcmp(argv[1], "color-balls") == 0) {
        int has_count = argc >= 6;
        int count = 0;
        if ((has_count && !parse_int(argv[5], 0, 400, &count)) ||
            (argc == 7 && !parse_unsigned(argv[6], EDEN_MAX_GAP_SAMPLES,
                                          &gap_samples)) ||
            !compose_color_balls(argv[2], argv[3], has_count, count, argv[4],
                                 gap_samples)) {
            return 2;
        }
        return 0;
    }
    usage(argv[0]);
    return 2;
}
