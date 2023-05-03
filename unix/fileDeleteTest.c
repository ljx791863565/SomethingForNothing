#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// 删除文件某一行
// TODO  是否可以删除多行


#define BUFSIZE 8192

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file> <del line no>\n", argv[0]);
        exit(1);
    }
    int DelLine = atoi(argv[2]);
    if (DelLine < 0) {
        fprintf(stderr, "Usage delete line Number\n");
        exit(1);
    }
    FILE *Rfp = fopen(argv[1], "r");
    if (Rfp == NULL) {
        perror("fopen()");
        exit(1);
    }

    FILE *Wfp = fopen(argv[1], "r+");
    if (Wfp == NULL) {
        fclose(Rfp);
        perror("fopen()");
        exit(1);
    }

    int line = 0;
    ssize_t offset = 0;
    ssize_t total_len = 0;

    size_t lineLen = 0;
    char *lineptr = NULL;

    fseek(Rfp, 0, SEEK_END);
    long filelen = ftell(Rfp);
    printf("%s wc = %ld\n", argv[1], filelen);
    fseek(Rfp, 0, SEEK_SET);

    while (line < (DelLine-1) && (offset = getline(&lineptr, &lineLen, Rfp)) > 0) {
        line++;
        total_len += offset;
        if (total_len >= filelen) {
            printf("file has no full line %d total_len = %ld\n", line, total_len);
            goto RETURN;
        }
        // printf("line = %d totel_len = %ld offset [%ld][%s]\n", line, total_len, offset, lineptr);
    }

    fseek(Wfp, total_len, SEEK_SET);

    offset = getline(&lineptr, &lineLen, Rfp);
    line++;
    printf("into line %d, len = %ld\n", line, offset);
    char del_buf[8192] = {0};
    size_t del_len = sprintf(del_buf, "%s", lineptr);
    while ((offset = getline(&lineptr, &lineLen, Rfp)) > 0) {
        fputs(lineptr, Wfp);
    }

    printf("delete line:%d [%ld][%s]\n", line, del_len, del_buf);

    int fd = fileno(Wfp);
    off_t newfilelen = (off_t)filelen - del_len; 
    ftruncate(fd, newfilelen);
    close(fd);

    printf("file %s ofter delete, wc = %ld\n", argv[1], newfilelen);
RETURN:
    free(lineptr);
    fclose(Wfp);
    fclose(Rfp);
    exit(0);
}