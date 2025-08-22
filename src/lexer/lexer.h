#include "../error.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

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
    // keywords
    VAR
    // clang-format on
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

bool initialized = false;

void froog_read_file(char *file_path, froog_file_info *f_info)
{
    assert(initialized == false);

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("file does not exits");
        exit(42);
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

void froog_token_append(froog_tokens *tokens, froog_token token)
{
    assert(initialized);

    tokens->tokens[tokens->length++] = token;
}

bool froog_match(froog_file_info *f_info, char *to_match)
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

froog_token froog_scrap_comment(froog_file_info *f_info)
{
    int ch;
    while ((ch = fgetc(f_info->file)) != FROOG_NEW_LINE) { }
    return COMMENT;
}

void froog_next(froog_file_info *f_info, froog_tokens *tokens)
{
    assert(initialized);

    int ch;
    froog_token t;
    while ((ch = fgetc(f_info->file)) != EOF) {
        switch (ch) {
        case ' ':
        case '\r':
        case '\t':
            break;
        case FROOG_NEW_LINE:
            f_info->lineno++;
        case '+':
            t = PLUS;
            break;
        case '-':
            t = MINUS;
            break;
        case '*':
            t = ASTERIKS;
            break;
        case '/':
            t = froog_match(f_info, "/") ? froog_scrap_comment(f_info) : SLASH;
            break;
        default:
            PANIC("Syntax error unknown: %c", ch)
        }
        froog_token_append(tokens, t);
    }
}
