#include "../error.h"
#include <assert.h>
#include <regex.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: maybe add windows \r\n
#define FROOG_NEW_LINE '\n'

typedef enum {
    // clang-format off
    // single characters
    PLUS, MINUS, SLASH, ASTERIKS, MODULO, EQUALS,
    // multi characters
    PLUS_EQUALS, MINUS_EQUALS, COMMENT,
    // literals
    IDENTIFIER, NUMBER, STRING,
    // keywords (_KEYWORD is generic to decide further which one)
    _KEYWORD, VAR
    // clang-format on
} froog_token_type;

// keep order for precendence
const char *identifier_patterns[] = {
    "^(?:[0-9](?:_?[0-9])*)(?:.[0-9](?:_?[0-9])*)?$", // number
    "^[A-Za-z][A-Za-z0-9_]*$", // keyword
    "^[A-Za-z_][A-Za-z0-9_]*$", // generic identifier
};
const froog_token_type identifier_mappings[] = {
    NUMBER, // number
    _KEYWORD, // keyword
    IDENTIFIER, // generic identifier
};

typedef struct {
    froog_token_type type;
    union {
        char *string;
        double number;
    } value;
    size_t lineno;
    size_t colno;
} froog_token;

typedef struct {
    size_t length;
    size_t capacity;
    froog_token tokens[];
} froog_tokens;

typedef struct {
    FILE *file;
    char *file_path;
    size_t len;
    size_t lineno;
    size_t colno;
} froog_file_info;

static bool initialized = false;

static void froog_token_append(froog_tokens *tokens, froog_token token)
{
    assert(initialized);

    tokens->tokens[tokens->length++] = token;
}

static bool froog_match(froog_file_info *f_info, char *to_match)
{
    size_t n = strlen(to_match);
    for (size_t i = 0; i < n; i++) {
        int ch = fgetc(f_info->file);
        if (ch == EOF || ch != to_match[i]) {
            return false;
        }
    }

    return true;
}

static froog_token_type froog_scrap_comment(froog_file_info *f_info)
{
    int ch;
    while ((ch = fgetc(f_info->file)) != FROOG_NEW_LINE) { }
    f_info->lineno++;
    f_info->colno = 1;
    return COMMENT;
}

static froog_token_type froog_get_identifer_type(char *s)
{
    regex_t regex;
    int ret;
    size_t len = sizeof(identifier_patterns) / sizeof(identifier_patterns[0]);
    for (size_t i = 0; i < len; i++) {
        ret = regcomp(&regex, identifier_patterns[i], REG_EXTENDED);

        if (ret)
            continue;

        ret = regexec(&regex, s, 0, NULL, 0);
        regfree(&regex);

        if (ret == 1) {
            return identifier_mappings[i];
        }
    }

    PANIC("Unreachable: at this point an identifier is expected")
}

static froog_token_type froog_keywords(char *keyword)
{
    if (strcmp(keyword, "var")) {
        return VAR;
    }

    PANIC("Unreachable: keyword not supported")
}

static char froog_peek(froog_file_info *f_info, char search)
{
    int ch = fgetc(f_info->file);
    if (ch == search) {
        // we consume the char because we treat it as handled by the logic that runs froog_peek
        return true;
    }
    // we don't want to consume the char because we might need to handle it elsewhere
    ungetc(ch, f_info->file);
    return false;
}

static void froog_consume(froog_file_info *f_info, char **s)
{
    int ch = fgetc(f_info->file);
}

static froog_token_type froog_match_symbols(froog_file_info *f_info, char ch)
{
    froog_token_type t;
    switch (ch) {
    case ' ':
    case '\r':
    case '\t':
        return true;
    case FROOG_NEW_LINE:
        f_info->lineno++;
        f_info->colno = 1;
    case '+':
        if (froog_peek(f_info, '=')) {
            t = PLUS_EQUALS;
        } else {
            t = PLUS;
        }
        break;
    case '-':
        if (froog_peek(f_info, '=')) {
            t = MINUS_EQUALS;
        } else {
            t = MINUS;
        }
        break;
    case '*':
        t = ASTERIKS;
        break;
    case '/':
        t = froog_match(f_info, "/") ? froog_scrap_comment(f_info) : SLASH;
        break;
    default:
        PANIC("Syntax error unknown: %c at line %d col %d", ch, (int)f_info->lineno, (int)f_info->colno);
    }

    return t;
}

static bool froog_next(froog_file_info *f_info, froog_tokens *tokens)
{
    void *value = { 0 };
    int ch = fgetc(f_info->file);
    if (ch == EOF)
        return false;

    froog_token_type t = froog_match_symbols(f_info, ch);

    froog_token token = {
        .type = t,
        .lineno = f_info->lineno,
        .colno = f_info->colno,
        .value = { value },
    };
    froog_token_append(tokens, token);
    return true;
}

void froog_read_file(char *file_path, froog_file_info *f_info)
{
    assert(initialized == false);

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        PANIC("File does not exist");
    }
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *f_info = (froog_file_info) {
        .file = file,
        .file_path = file_path,
        .len = len,
        .lineno = 1,
        .colno = 1
    };

    initialized = true;
}

void froog_close_file(const froog_file_info *f_info)
{
    assert(f_info->file != NULL);

    fclose(f_info->file);
}

void froog_lex(froog_file_info *f_info, froog_tokens *tokens)
{
    assert(initialized);

    while (froog_next(f_info, tokens)) { }
}
