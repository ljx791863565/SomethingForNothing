#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage:  %s <src_file> <dest_file>\n", argv[0]);
        exit(1);
    }

    char buf[BUFSIZE] = {0};
    FILE *fps = fopen(argv[1], "r");
    if (fps == NULL) {
        perror("fopen()");
        exit(1);
    }

    FILE *fpd = fopen(argv[2], "w");
    if (fpd == NULL) {
        perror("fopen()");
        fclose(fps);
        exit(1);
    }

    while (fgets(buf, BUFSIZE, fps) != NULL) {
        fputs(buf, fpd);
    }

    fclose(fpd);
    fclose(fps);

    exit(0);
}