#include "../kaede.h"
#include <stdbool.h>

bool consume(char *op, TokenKind token_kind);
void expect(char *op);
int expect_number();
Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_ternary(NodeKind kind, Node *lhs, Node *chs, Node *rhs);
Node *new_num(int val);
void add_node_to_block(Node *blk, Node *node);
void add_node_to_args(Node *func, Node *node);

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *relational();

void defined_check(Node *node);
LVar *find_lvar();
Token *consume_ident();

