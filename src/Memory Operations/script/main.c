#include <stdio.h>

int main() {
    for (int i = 0; i < 10000; i++) {
        printf("DOIT(%d) ", 4 * i);
    }
    printf("\n");
}