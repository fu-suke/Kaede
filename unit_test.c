#include "kaede.h"

int main(int argc, char **argv) {
    char *test_kind = argv[1];
    user_input = argv[2];
    token = tokenize();
    if (strcmp(test_kind, "tokenize") == 0) {
        while(token->next != NULL) {
            for(int i=0; i<token->len; i++){
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
    }
    return 0;
}