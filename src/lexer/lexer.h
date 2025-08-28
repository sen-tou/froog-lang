#pragma once
#include <stdint.h>
#include <stdio.h>

// TODO: maybe add windows \r\n
#define FROOG_NEW_LINE '\n'
#define FROOG_INDETIFER_MAX_SIZE 64

#define NAMES C(PLUS) C(MINUS) C(SLASH) C(ASTERIKS) C(MODULO) C(EQUALS) C(LESS_THAN) C(GREATER_THAN) \
    C(PLUS_EQUALS) C(MINUS_EQUALS) C(COMMENT)                                                        \
        C(IDENTIFIER) C(NUMBER) C(STRING)                                                            \
            C(_KEYWORD) C(VAR)

#define C(x) x,

typedef enum { NAMES TOP } froog_token_type;

#undef C

#define C(x) #x,

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
    froog_token *tokens;
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

froog_tokens *froog_create_tokens(size_t init_capacity);

void froog_free_tokens(froog_tokens *ft);

void froog_intermediate_representation(froog_tokens *ft);
