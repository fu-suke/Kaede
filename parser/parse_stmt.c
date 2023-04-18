#include "parse.h"

// stmt = expr ';'
// | "return" expr ";"
// | "if" "(" expr ")" stmt ("else" stmt)?
// | "while" "(" expr ")" stmt
// | "for" "(" expr? ";" expr? ";" expr? ")" stmt
// | "{" stmt+ "}"
Node *expr();

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