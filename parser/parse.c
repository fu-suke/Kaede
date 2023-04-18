#include "parse.h"

Token *token;
LVar *locals; // ローカル変数

// Consumes the current token if it matches `op`.
bool consume(char *op, TokenKind token_kind) {
    if (token->kind != token_kind || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// Ensure that the current token is TK_NUM.
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, 0, "数字が入ります");
    int val = token->val;
    token = token->next;
    return val;
}
Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    else {
        Token *tmp = token;
        token = token->next;
        return tmp;
    }
};

bool at_eof() { return token->kind == TK_EOF; }


void add_node_to_block(Node *blk, Node *node) {
    if (!node) {
        return;
    }
    if (!blk->body) {
        blk->body = node;
        return;
    }
    Node *current = blk->body;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
}

void add_node_to_args(Node *func, Node *node) {
    if (!node) {
        return;
    }
    if (func->kind != ND_FUNC_CALL) {
        error_at(token->str, 0, "関数ではありません");
    }
    if (!func->args) {
        func->args = node;
        return;
    }
    Node *current = func->args;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
}

bool is_const(char *str, int len) {
    // 定数の1文字目は [A-Z_]
    if (!isupper(str[0]) && str[0] != '_') {
        return false;
    }
    // 定数の2文字目以降は [A-Z0-9_]
    for (int i = 1; i < len; i++) {
        if (!isupper(str[i]) && !isdigit(str[i]) && str[i] != '_') {
            return false;
        }
    }
    return true;
}

Node *code[100];

// program = function+
void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = function_def();
    }
    code[i] = NULL;
}

// function = ident "(" ")" "{" stmt* "}"
Node *function_def() {
    locals = calloc(1, sizeof(LVar));
    Node *node = new_node(ND_FUNC_DEF);

    ///// ここから実質 expect_function()
    Token *tok = consume_ident();
    // 関数名の取得
    char *name = calloc(1, (tok->len + 1) * sizeof(char));
    strncpy(name, tok->str, tok->len);
    node->func_name = name;

    expect("(");
    // 引数の処理
    expect(")");
    expect("{");

    while (!consume("}", TK_RESERVED)) {
        add_node_to_block(node, stmt());
    }
    ///// ここまで

    node->locals = locals;
    return node;
}

LVar *find_lvar(Token *tok, Node *node) {
    if (tok != NULL){
        for (LVar *var = locals; var; var = var->next)
            if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
                return var;
        return NULL;
    }
    if (node != NULL){
        for(LVar *var = locals; var; var = var->next){
            if (node->offset == var->offset)
                return var;
        }
        return NULL;
    }
    fprintf(stderr, "%s\n", "find_lvar の引数が両方NULLです");
}

