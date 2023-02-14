#include "kaede.h"

void show_code_i(Node *);
void show_block(Node *);

void *show_node(Node *node) {
    int d = node->kind;
    switch (d) {
    case 0:
        printf("+");
        break;
    case 1:
        printf("-");
        break;
    case 2:
        printf("*");
        break;
    case 3:
        printf("/");
        break;
    case 4:
        printf("==");
        break;
    case 5:
        printf("!=");
        break;
    case 6:
        printf("<");
        break;
    case 7:
        printf("<=");
        break;
    case 8:
        printf("%d", node->val);
        break;
    case 9:
        printf("=");
        break;
    case 10:
        printf("offset(%d)", node->offset);
        break;
    case 11:
        printf("return");
        break;
    case 12:
        printf("if");
        break;
    case 13:
        printf("while");
        break;
    case 14:
        printf("\n");
        printf("{");
        show_block(node->body);
        printf("}");
        break;
    default:
        return "";
    }
}

void show_code_i(Node *node) {
    if (!node) {
        return;
    }
    show_node(node);
    if (!node->lhs || !node->chs || !node->rhs) {
        printf("[");
        show_code_i(node->lhs);
        printf(",");
        show_code_i(node->chs);
        printf(",");
        show_code_i(node->rhs);
        printf("]");
    }
}

void show_block(Node *node) {
    if (!node)
        return;
    show_code_i(node);
    printf("\n");
    show_block(node->next);
}

void show_code() {
    int i = 0;
    while (1) {
        if (!code[i]) {
            break;
        }
        show_code_i(code[i]);
        printf("\n");
        i++;
    }
    printf("------------------\n");
}

int main(int argc, char **argv) {
    char *test_kind = argv[1];
    user_input = argv[2];
    token = tokenize();
    if (strcmp(test_kind, "tokenize") == 0) {
        while (!token->next) {
            for (int i = 0; i < token->len; i++) {
                printf("%c", token->str[i]);
            }
            printf(" ");
            token = token->next;
        }
        printf("\n");
    }
    if (strcmp(test_kind, "parse") == 0) {
        locals = calloc(1, sizeof(LVar));
        locals->next = NULL;
        program();
        show_code();
    }
    return 0;
}