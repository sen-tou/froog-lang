#pragma once
#include <assert.h>
#include <regex.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: maybe add windows \r\n
#define FROOG_NEW_LINE '\n'
#define FROOG_INDETIFER_MAX_SIZE 64

typedef enum {
    // clang-format off
    // single characters
    PLUS, MINUS, SLASH, ASTERIKS, MODULO, EQUALS, LESS_THAN, GREATER_THAN,
    // multi characters
    PLUS_EQUALS, MINUS_EQUALS, COMMENT,
    // literals
    IDENTIFIER, NUMBER, STRING,
    // keywords (_KEYWORD is generic to decide further which one)
    _KEYWORD, VAR
    // clang-format on
} froog_token_type;

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

void froog_read_file(char *file_path, froog_file_info *f_info);

void froog_close_file(const froog_file_info *f_info);

void froog_lex(froog_file_info *f_info, froog_tokens *tokens);
