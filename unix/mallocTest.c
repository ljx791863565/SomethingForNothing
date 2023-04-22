#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    // 对于malloc之类的void*返回值，可以直接赋值给任意类型指针而无需强转
    // 告警只是因为可能没有包含stdlib头文件
    char *p = malloc(100);
    if (p == NULL) {
        perror("malloc");
        exit (1);
    }
    fprintf(stdout, "malloc 100 OK\n");

    exit(0);
}