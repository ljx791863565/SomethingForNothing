#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // FILE *fp = fopen("test", "r");
    // if (fp == NULL) {
    //     // fprintf(stderr, "fopen failed. %s\n", strerror(errno));
    //     perror("fopen()");
    //     exit(1);
    // }
    // fprintf(stdout, "OK!\n");

    // 测试一个进程可以打开的最大fp  #1021  加上STDIN STDOUT STDERR
    // ulimit -a 可得
    // open files                      (-n) 1024
    int count = 0;
    FILE *fp = NULL;
    while (1) {
        fp = fopen("test", "w");
        if (fp == NULL)
        {
            perror("fopen()");
            break;
        }
        count++;
    }
    printf("count = %d\n", count);
    fclose(fp);
    exit(0);
}