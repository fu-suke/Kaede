#include "kaede.h"

Token *token;
// ローカル変数
LVar *locals;

// Consumes the current token if it matches `op`.
bool consume(char *op, TokenKind token_kind) {
    // if ((token->kind != TK_RESERVED && token->kind != TK_RETURN &&
    //      token->kind != TK_IF && token->kind != TK_ELSE) ||
    if (token->kind != token_kind || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// Ensure that the current token is `op`.
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "expected \"%s\"", op);
    token = token->next;
}

// Ensure that the current token is TK_NUM.
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->chs = NULL;
    node->rhs = rhs;
    return node;
}

Node *new_ternary(NodeKind kind, Node *lhs, Node *chs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->chs = chs;
    node->rhs = rhs;
    return node;
}
Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}
Node *code[100];
Node *stmt();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// program    = stmt*
void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = stmt();
    }

    code[i] = NULL;
}

// stmt = expr ';' | "return" expr ";" |"if" "(" expr ")" stmt ("else" stmt)?  |
// "while" "(" expr ")" stmt
Node *stmt() {
    Node *node;
    if (consume("return", TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else if (consume("if", TK_IF)) {
        expect("(");
        node = new_ternary(ND_IF, expr(), NULL, NULL);
        expect(")");
        node->chs = stmt(); // true
        if (consume("else", TK_ELSE)) {
            node->rhs = stmt(); // false
        }
        return node;
    } else if (consume("while", TK_WHILE)) {
        expect("(");
        node = new_binary(ND_WHILE, expr(), NULL);
        expect(")");
        node->rhs = stmt();
        return node;
    } else {
        node = expr();
    }

    if (!consume(";", TK_RESERVED))
        error_at(token->str, "';'ではないトークンです");
    return node;
}

// expr = assign
Node *expr() { return assign(); }

// equality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==", TK_RESERVED))
            node = new_binary(ND_EQ, node, relational());
        else if (consume("!=", TK_RESERVED))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<", TK_RESERVED))
            node = new_binary(ND_LT, node, add());
        else if (consume("<=", TK_RESERVED))
            node = new_binary(ND_LE, node, add());
        else if (consume(">", TK_RESERVED))
            node = new_binary(ND_LT, add(), node);
        else if (consume(">=", TK_RESERVED))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+", TK_RESERVED))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-", TK_RESERVED))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*", TK_RESERVED))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/", TK_RESERVED))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? unary
//       | primary
Node *unary() {
    if (consume("+", TK_RESERVED))
        return unary();
    if (consume("-", TK_RESERVED))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

//
Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    else {
        Token *tmp = token;
        token = token->next;
        return tmp;
    }
};

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

// num | ident | "(" expr ")"
Node *primary() {
    if (consume("(", TK_RESERVED)) {
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_num(expect_number());
}
