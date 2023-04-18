#include "kaede.h"

// Reports an error location and exit.
void error_at(char *loc, int r, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input - r;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// Ensure that the current token is `op`.
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, 0, "\"%s\"が入ります", op);
    token = token->next;
}

void defined_check(Node *node){
    if (node){
        if (node->kind == ND_LVAR){
            LVar *var = find_lvar(NULL, node);
            if (!var->is_filled)
                error_at(token->str, 0, "変数が未定義です");
        }
        defined_check(node->lhs);
        defined_check(node->rhs);
    }
}