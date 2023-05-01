#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 获取一个5G大小的文件用于测试

#define MAX_K 1024 *1024 * 500

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tmpfile>\n", argv[0]);
        exit(1);
    }


    FILE *fp = fopen(argv[1], "w");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }

    unsigned int seed = time(NULL);
    srand(seed);
    int i = 0;
    char buf[8] = {0};
    for (i = 0; i < MAX_K; i++) {
        snprintf(buf, 8, "%d", rand());
        // printf("num = [%s]\n", buf);
        fputs(buf, fp);
    }


    fclose(fp);

    exit(1);
}