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
        error_at(token->str, 0, "\"%s\"が入ります", op);
    token = token->next;
}

// Ensure that the current token is TK_NUM.
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, 0, "数字が入ります");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->body = NULL;
    node->args = NULL;
    node->next = NULL; // 追加
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

Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    else {
        Token *tmp = token;
        token = token->next;
        return tmp;
    }
};

void add_node_to_block(Node *blk, Node *node) {
    if (!node) {
        return;
    }
    // if (blk->kind != ND_BLOCK) {
    //     error_at(token->str, 0, "ブロックではありません");
    // }
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
        // code[i++] = stmt();
        code[i++] = function_def();
    }
    code[i] = NULL;
}

Node *function_def() {
    locals = calloc(1, sizeof(LVar));
    Node *node = new_node(ND_FUNC_DEF);
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
    node->locals = locals;
    return node;
}
// stmt = expr ';'
// | "return" expr ";"
// | "if" "(" expr ")" stmt ("else" stmt)?
// | "while" "(" expr ")" stmt
// | "for" "(" expr? ";" expr? ";" expr? ")" stmt
// | "{" stmt+ "}"

Node *stmt() {
    Node *node;
    if (consume("{", TK_RESERVED)) {
        node = new_node(ND_BLOCK);
        Node *current = node;
        if (consume("}", TK_RESERVED)) {
            error_at(token->str, 2, "{}の中に文がありません");
        }
        Node *new = stmt();
        current->body = new;
        current = current->body;
        while (!consume("}", TK_RESERVED)) {
            Node *new = stmt();
            current->next = new;
            current = new;
        }
        return node;
    }

    else if (consume("return", TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        // if (A) B; else C;
    } else if (consume("if", TK_IF)) {
        expect("(");
        node = new_ternary(ND_IF, expr(), NULL, NULL);
        expect(")");
        node->chs = stmt(); // true
        if (consume("else", TK_ELSE)) {
            node->rhs = stmt();
        } // false
        return node;
    } else if (consume("while", TK_WHILE)) {
        expect("(");
        node = new_binary(ND_WHILE, expr(), NULL);
        expect(")");
        node->rhs = stmt();
        return node;
    } else if (consume("for", TK_FOR)) {

        expect("(");
        Node *init = NULL, *condition, *step = NULL, *sentence = NULL;
        // init
        if (!consume(";", TK_RESERVED)) {
            init = expr();
            expect(";");
        }
        // condition
        if (!consume(";", TK_RESERVED)) { // condition あり
            condition = expr();           // i < 5
            expect(";");
        } else { // condition なし
            // 必ず true になるようなノードを適当に作る
            condition = new_binary(ND_EQ, new_num(1), new_num(1));
        }
        // step
        if (!consume(")", TK_RESERVED)) { // step あり
            step = expr();
            expect(")");
        }
        // sentence
        sentence = stmt();

        Node *node = new_node(ND_BLOCK);
        add_node_to_block(node, init);
        Node *blk = new_node(ND_BLOCK);
        Node *loop = new_binary(ND_WHILE, condition, blk);
        add_node_to_block(blk, sentence);
        add_node_to_block(blk, step);
        add_node_to_block(node, loop);
        return node;

    } else {
        node = expr();
    }

    if (!consume(";", TK_RESERVED)) {
        error_at(token->str, 0, "';'ではないトークンです");
    }
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

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

//   num
// | ident ("("")")?
// | "(" expr ")"
Node *primary() {
    // ( 式 )
    if (consume("(", TK_RESERVED)) {
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *tok = consume_ident();
    if (tok) { // 変数名 or 関数名がきたとき
        Node *node = new_node(ND_LVAR);
        // LVar *lvar = calloc(1, sizeof(LVar));
        // FUNCと判断するため
        // consume("(") が true だったら node->kind = ND_FUNC_CALL; else ND_LVAR

        // 関数名を退避
        char *name = calloc(1, (tok->len + 1) * sizeof(char));
        strncpy(name, tok->str, tok->len);
        name[tok->len] = '\0';

        // 関数呼び出し
        if (consume("(", TK_RESERVED)) {
            node->kind = ND_FUNC_CALL;
            node->func_name = name;

            if (!consume(")", TK_RESERVED)) {
                add_node_to_args(node, add());
                while (consume(",", TK_RESERVED)) { // 引数が複数ある場合
                    add_node_to_args(node, add());
                }
                expect(")");
            }

            return node;
        }
        LVar *lvar = find_lvar(tok); // 変数名が見つかったらそのアドレスを返す

        // 変数
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            // printf("hoge\n");
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_num(expect_number());
}
