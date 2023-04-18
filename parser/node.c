#include "parse.h"

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->body = NULL;
    node->args = NULL;
    node->next = NULL;
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