#include "kaede.h"
int begincount = 0;
int endcount = 0;
int elsecount = 0;

char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {

    switch (node->kind) {
    case ND_FUNC:
        // 引数を評価したものをスタックに積む

        int args_count = 0; // 引数の数を数える
        Node *current = node->args;
        while (current != NULL) {
            args_count++;
            gen(current); // 引数を評価してスタックに積む
            current = current->next; // 次の引数へ
        }
        for (; args_count > 0; args_count--) {
            printf("  pop %s\n",
                   arg_reg[args_count - 1]); // 引数をレジスタに代入していく
        }

        printf("  call %s\n", node->func_name); // 関数を呼ぶ
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    // if (A) B else C
    case ND_IF:
        gen(node->lhs); // A をコンパイルしたコード
        printf(
            "  pop rax\n"); // A
                            // の結果がスタックトップに残っているのでとってくる
        printf("  cmp rax, 0\n");
        // ソースコードのif文にelseが無いときはパーザで強制的にelse
        // 0;を追加している
        /*
        if (node->rhs == NULL) { // else がないとき
            int tmp = endcount;
            printf("  je  .Lend%d\n", endcount++); // rax が 0 なら if を抜ける
            gen(node->chs); // B をコンパイルしたコード
            printf(".Lend%d:\n", tmp);
        } else
        */
        {
            // printf("hoge\n");
            int tmp_else = elsecount;
            printf("  je  .Lelse%d\n", elsecount++);
            gen(node->chs); // B をコンパイルしたコード
            int tmp_end = endcount;
            printf("  jmp .Lend%d\n", endcount++);
            printf(".Lelse%d:\n", tmp_else);
            gen(node->rhs); // C をコンパイルしたコード
            printf(".Lend%d:\n", tmp_end);
        }
        return;
    // while (A) B
    case ND_WHILE: {
        int tmp_begin = begincount;
        printf(".Lbegin%d:\n", tmp_begin);
        gen(node->lhs);        // condition
        printf("  pop rax\n"); // condition を rax に pop
        printf("  cmp rax, 0\n");
        int tmp_end = endcount;
        printf("  je  .Lend%d\n", endcount++);
        gen(node->rhs);
        printf("  jmp .Lbegin%d\n", begincount++);
        printf(".Lend%d:\n", tmp_end);
        return;
    }
    case ND_BLOCK: {
        Node *current = node->body;
        while (current != NULL) {
            gen(current);
            current = current->next;
            if (current != NULL) {
                printf("  pop rax\n");
            }
        }
        return;
    }
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
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
    case ND_EQ: // ==
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE: // !=
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT: // <
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE: // <=
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}