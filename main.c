#include "kaede.h"

char *user_input;





//
// Code generator
//

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
        error("%s: invalid number of arguments", argv[0]);

    // Tokenize and parse.
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // Print out the first half of assembly.
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Traverse the AST to emit assembly.
    gen(node);

    // A result must be at the top of the stack, so pop it
    // to RAX to make it a program exit code.
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}