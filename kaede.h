#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Tokenizer
//

typedef enum {
    TK_RESERVED, // Keywords or punctuators
    TK_IDENT,    // Identifier
    TK_NUM,      // Integer literals
    TK_EOF,      // End-of-file markers
    TK_RETURN,   // return
    TK_IF,       // if
    TK_ELSE,     // else
    TK_WHILE,    // while
    TK_FOR,      // for
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
    TokenKind kind; // Token kind
    Token *next;    // Next token
    int val;        // If kind is TK_NUM, its value
    char *str;      // Token string
    int len;        // Token length
};

// Input program
extern char *user_input;

// Current token
extern Token *token;

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

Token *tokenize();

//
// Parser
//

typedef enum {
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_NUM,    // Integer
    ND_ASSIGN, // =
    ND_LVAR,   // ローカル変数
    ND_RETURN, // return
    ND_IF,
    ND_WHILE,
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
    NodeKind kind; // Node kind
    Node *lhs;     // Left-hand side
    Node *chs;     // Center side
    Node *rhs;     // Right-hand side
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
};

Node *expr();

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

extern LVar *locals;

// Code generator
void gen(Node *node);

extern Node *code[];
void program();