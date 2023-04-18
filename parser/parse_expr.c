#include "parse.h"

// expr = assign*
Node *expr() { return assign(); }

// assign = 
// | equality ("=" assign)?
// | equality ("+=" mul)?
// | equality ("-=" mul)? 
// | equality ("*=" unary)?
// | equality ("/=" unary)?
Node *assign() {
    Node *node = equality();
    LVar *tmp;
    bool is_assigned = false;  // 変数の代入が行われたかどうか
    if (consume("=", TK_RESERVED)){
        // "="がある文は必ず代入文で左辺は変数なので、代入が必ず行われる
        is_assigned = true;
        node = new_binary(ND_ASSIGN, node, assign());
        // 定数の再代入をチェックする
        LVar *lvar = find_lvar(NULL, node->lhs);
        tmp = lvar;
        if (lvar->is_filled){
            if (lvar->is_const)
                error_at(token->str, 0, "定数に再代入を行っています");
        }
    }
    if (consume("+=", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_ADD, node, mul()));
    if (consume("-=", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_SUB, node, mul()));
    if (consume("*=", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_MUL, node, unary()));
    if (consume("/=", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_DIV, node, unary()));
    if (consume("++", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_ADD, node, new_num(1)));
    if (consume("--", TK_RESERVED))
        node = new_binary(ND_ASSIGN, node, new_binary(ND_SUB, node, new_num(1)));
    if (node->kind == ND_ASSIGN){
        if(node->lhs->kind != ND_LVAR){
            error_at(token->str, 0, "代入の左辺値が変数ではありません");
        }
        defined_check(node->rhs);
    }
    if (is_assigned) tmp->is_filled = true;
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
        LVar *lvar = find_lvar(tok, NULL); // 変数名が見つかったらそのアドレスを返す

        // 変数
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->is_filled = false;
            lvar->is_const = is_const(tok->str, tok->len);
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_num(expect_number());
}