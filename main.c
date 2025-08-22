#include "src/lexer/lexer.h"
#include <assert.h>

int main(int argc, char **argv)
{
    assert(argc >= 2);

    char *file_path = argv[1];
    froog_file_info f_info = { 0 };
    froog_tokens tokens = { 0 };

    froog_read_file(file_path, &f_info);

    froog_next(&f_info, &tokens);

    froog_close_file(&f_info);
}
