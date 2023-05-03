#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int filelen_fgetc(const char *filename, long *len)
{
    if (len == NULL || filename == NULL) {
        fprintf(stderr, "filename or len is NULL\n");
        return -1;
    }
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen()");
        return -1;
    }

    long count = 0;

    while (fgetc(fp) != EOF) {
        count++;
    }
    fclose(fp);
    *len = count;
    return 0;
}

int filelen_fseek(const char *filename, long *len)
{
    // 使用fseek 和 ftell获取文件大小
    if (len == NULL || filename == NULL) {
        fprintf(stderr, "filename or len is NULL\n");
        return -1;
    }
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen()");
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    long count = ftell(fp);
    *len = count;
    fclose(fp);
    return 0;
}

int filelen_stat(const char *filename, off_t *len)
{
    // stat st_size
    if (len == NULL || filename == NULL) {
        fprintf(stderr, "filename or len is NULL\n");
        return -1;
    }
    struct stat st;
    int ret = stat(filename, &st);
    if (ret == -1) {
        perror("stat()");
        return -1;
    }
    *len = st.st_size;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    long len1, len2;
    off_t len3;

    if (filelen_fgetc(argv[1], &len1) != -1) {
        printf("len1 = %ld\n", len1);
    }

    if (filelen_fseek(argv[1], &len2) != -1) {
        printf("len1 = %ld\n", len2);
    }

    if (filelen_stat(argv[1], &len3) != -1) {
        printf("len1 = %ld\n", len3);
    }

    exit(0);
}