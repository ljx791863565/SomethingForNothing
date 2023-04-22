#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }

    int count = 0;

    while (fgetc(fp) != EOF) {
        count++;
    }

    fprintf(stdout, "count = %d\n", count);

    fclose(fp);

    exit(0);
}