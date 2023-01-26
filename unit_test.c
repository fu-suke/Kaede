#include "kaede.h"

int main(int argc, char **argv) {
    char *test_kind = argv[1];
    user_input = argv[2];
    token = tokenize();
    if (strcmp(test_kind, "parse") == 0) {
        locals = calloc(1, sizeof(LVar));
        locals->next = NULL;
        program();
    }
    return 0;
}