#include "error.h"
#include "lexer/lexer.h"
#include <assert.h>

int main(int argc, char **argv)
{
    assert(argc >= 2);

    char *file_path = argv[1];
    froog_file_info f_info = { 0 };

    froog_read_file(file_path, &f_info);

    froog_tokens *tokens = froog_create_tokens(f_info.len);
    froog_lex(&f_info, tokens);
    froog_intermediate_representation(tokens);
    froog_free_tokens(tokens);

    froog_close_file(&f_info);

    return 0;
}
