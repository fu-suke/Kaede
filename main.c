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
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    // printf("main:\n");

    // プロローグ
    // printf(" # プロローグ\n");
    // push("rbp");
    // printf("  mov rbp, 面白そうrsp\n");

    // 変数個分の領域を確保する
    // int mem = locals_len * 8;
    // printf("  sub rsp, %d\n", mem);

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        printf("# 木の始まり\n");

        gen_stmt(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        // printf("  pop rax #木の終わり\n");
        printf("# 木の終わり\n");
    }
    free(locals);
    return 0;
}