#include "kaede.h"
int begincount = 0;
int endcount = 0;
int elsecount = 0;
int stack_depth = 0; // call 命令が来るときに必ず rsp が 16 の倍数になるようにする

char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void pop(char *reg){
    printf("  pop %s\n", reg);
    stack_depth--;
    printf("# depth:%d\n", stack_depth);
}

void push(char *reg){
    printf("  push %s\n", reg);
    stack_depth++;
    printf("# depth:%d\n", stack_depth);
}

void push_num(int num){
    printf("  push %d\n", num);
    stack_depth++;
    printf("# depth:%d\n", stack_depth);
}

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    push("rax");
}

void gen_stmt(Node *node){
    switch (node->kind){
        // if (A) B else C
    case ND_IF:{
        gen(node->lhs); // A をコンパイルしたコード
        pop("rax");
                            // の結果がスタックトップに残っているのでとってくる
        printf("  cmp rax, 0\n");

        if (node->rhs == NULL) { // else がないとき
            int tmp = endcount;
            printf("  je  .Lend%d\n", endcount++); // rax が 0 なら if を抜ける
            gen_stmt(node->chs); // B をコンパイルしたコード
            printf(".Lend%d:\n", tmp);
        } else { // else があるとき
            int tmp_else = elsecount;
            printf("  je  .Lelse%d\n", elsecount++);
            gen_stmt(node->chs); // B をコンパイルしたコード
            int tmp_end = endcount;
            printf("  jmp .Lend%d\n", endcount++);
            printf(".Lelse%d:\n", tmp_else);
            gen_stmt(node->rhs); // C をコンパイルしたコード
            printf(".Lend%d:\n", tmp_end);
        }
        return;
    }
        case ND_WHILE: {
            int tmp_begin = begincount;
            printf(".Lbegin%d:\n", tmp_begin);
            gen(node->lhs);        // condition
            pop("rax");
            printf("  cmp rax, 0\n");
            int tmp_end = endcount;
            printf("  je  .Lend%d\n", endcount++);
            gen_stmt(node->rhs); // スタックトップに値が残る
            printf("  jmp .Lbegin%d\n", begincount++);
            printf(".Lend%d:\n", tmp_end);
            return;
    }
    case ND_BLOCK: {
        Node *current = node->body;
        while (current != NULL) {
            gen_stmt(current);
            current = current->next;
        }
        return;
    }
    default:
        gen(node);
        pop("rax");
        return;
    }

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
            pop(arg_reg[args_count - 1]);
        }

        /*　こ↑こ↓　*/
        if (stack_depth % 2 == 0){
            push("rsp");
            printf("  call %s\n", node->func_name);
            pop("rsp");
        }
        else{
            printf("  call %s\n", node->func_name); // 関数を呼ぶ
        }
        push("rax");
        return;
    case ND_RETURN:
        gen(node->lhs);
        pop("rax");
        printf("  mov rsp, rbp\n");
        pop("rbp");
        printf("  ret\n");
        return;
    case ND_NUM:
        push_num(node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        pop("rax");
        printf("  mov rax, [rax]\n");
        push("rax");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        pop("rdi");
        pop("rax");
        printf("  mov [rax], rdi\n");
        push("rdi");
        return;
    
    }

    gen(node->lhs);
    gen(node->rhs);

    pop("rdi");
    pop("rax");

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

    push("rax");
}