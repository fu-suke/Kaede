#include "stdio.h"

int foo() { printf("kani\n"); }
int sum(int x, int y) { printf("The sum of %d and %d is %d.\n", x, y, x + y); }
int sub(int x, int y) { printf("The gap of %d and %d is %d.\n", x, y, x - y); }
int triple(int x, int y, int z) { printf("%d\n", x + y + z); }
int six(int x, int y, int z, int a, int b, int c) {
    printf("%d\n", x + y + z - a - b - c);
}