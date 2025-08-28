#include "lexer.h"
#include "../error.h"
#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// keep order for precendence
static const char *identifier_patterns[] = {
    "^[0-9](_?[0-9])*(\\.[0-9](_?[0-9])*)?$", // number
    "^[A-Za-z][A-Za-z0-9_]*$", // generic identifier (vars, keywords)
};
static const froog_token_type identifier_mappings[] = {
    NUMBER, // number
    IDENTIFIER, // identifier
};

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
    size_t ch;
    while ((ch = fgetc(f_info->file)) != FROOG_NEW_LINE) { }
    f_info->lineno++;
    f_info->colno = 1;
    return COMMENT;
}

static froog_token_type froog_get_identifer_type(char *s)
{
    regex_t regex;
    size_t ret;
    size_t len = sizeof(identifier_patterns) / sizeof(identifier_patterns[0]);
    for (size_t i = 0; i < len; i++) {
        ret = regcomp(&regex, identifier_patterns[i], REG_EXTENDED);

        if (ret != 0)
            continue;

        ret = regexec(&regex, s, 0, NULL, 0);
        regfree(&regex);

        if (ret == 0) {
            return identifier_mappings[i];
        }
    }

    PANIC("Unreachable: at this point an identifier is expected, tripped on %s", s)
}

static froog_token_type froog_keywords(char keyword[])
{
    if (strcmp(keyword, "var") == 0) {
        return VAR;
    }

    return IDENTIFIER;
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

static void froog_consume(froog_file_info *f_info, char buf[], size_t buf_size)
{
    size_t consume_counter = buf_size;

    while (true) {
        int ch = fgetc(f_info->file); // use int to properly check EOF
        if (isspace(ch) || ch == EOF) {
            buf[consume_counter] = '\0'; // null terminate string
            return;
        }

        if (consume_counter >= FROOG_INDETIFER_MAX_SIZE - 1) {
            PANIC("identifiers can't be longer than %d characters", FROOG_INDETIFER_MAX_SIZE);
        }

        buf[consume_counter] = (char)ch; // append character
        consume_counter++;
    }
}

static froog_token_type froog_match_symbols(froog_file_info *f_info, int ch)
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
    case '=':
        t = EQUALS;
        break;
    case '%':
        t = MODULO;
        break;
    case '<':
        t = LESS_THAN;
        break;
    case '>':
        t = GREATER_THAN;
        break;
    default:
        char s[FROOG_INDETIFER_MAX_SIZE];
        s[0] = ch; // the character is already consumed so we need to add at first position
        froog_consume(f_info, s, 1);
        froog_token_type id_type = froog_get_identifer_type(s);
        t = id_type;

        if (id_type == IDENTIFIER) {
            froog_token_type keyword = froog_keywords(s);
            t = keyword;
        }
        break;
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
