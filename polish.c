#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// fake readline function

char *readline(char *prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

// fake add_history function

void add_history(char *unused) {}

// otherwise include the editline header

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(void)
{
    // create parsers
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    // define them with the following language

    mpca_lang(MPCA_LANG_DEFAULT,
              "                                                   \
        number : /-?[0-9]+/;                            \
        operator : '+' | '-' | '*' | '/';               \
        expr : <number> | '(' <operator> <expr>+ ')';   \
        lispy : /^/ <operator> <expr>+ /$/;             \
    ",
              Number, Operator, Expr, Lispy);

    // Print welcome message
    puts("Lispy version 0.0.0.0.2");
    puts("Press ctrl + c to Exit\n");

    // REPL Loop
    while (1)
    {
        char *input = readline("lispy> ");
        add_history(input);

        // try to parse the input

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r))
        {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // free input after use
        free(input);
    }

    // clean up parsers
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}