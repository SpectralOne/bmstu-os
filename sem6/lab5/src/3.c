#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define FILENAME "out.txt"

int main() {
    FILE *f1 = fopen(FILENAME, "w");
    FILE *f2 = fopen(FILENAME, "w");

    for (char c = 'a'; c <= 'z'; c++) {
        if (c % 2) {
            fprintf(f1, "%c", c);
        } else {
            fprintf(f2, "%c", c);
        }
    }

    fclose(f2);
    fclose(f1);

    return 0;
}
