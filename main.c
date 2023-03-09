#include "kaede.h"

void display_token(Token *token) {
    printf("token is \n");
    while (token->kind != TK_EOF) {
        printf("%s  ", token->str);
        printf("%d\n", token->kind);
        token = token->next;
    }
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    // トークナイズしてパースする
    // 結果はcodeに保存される
    user_input = argv[1];
    token = tokenize();
    // display_token(token);
    locals = calloc(1, sizeof(LVar));
    locals->next = NULL;
    program();

    int locals_len = -1;
    while (locals != NULL) {
        locals_len++;
        locals = locals->next;
    }
    // locals_lenを偶数に切り上げる
    locals_len = (locals_len + 1) / 2 * 2;

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // // プロローグ
    printf(" # プロローグ\n");
    // printf("  push rbp\n");
    push("rbp");
    printf("  mov rbp, rsp\n");

    // 変数個分の領域を確保する
    int mem = locals_len * 8;
    printf("  sub rsp, %d\n", mem);


    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        printf("# 木の始まり\n");
        gen_stmt(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        // printf("  pop rax #木の終わり\n");
        printf("# 木の終わり\n");
    }

    // エピローグ
    printf(" # エピローグ\n");
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    // printf("  pop rbp\n");
    pop("rbp");
    printf("  ret\n");
    free(locals);
    return 0;
}