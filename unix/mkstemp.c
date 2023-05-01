#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void make_temp(char *template)
{
    int fd;
    struct stat sbuf;

    if ((fd = mkstemp(template)) < 0) {
        perror("mkdremp()");
        exit(1);
    }
    printf("temp name = %s\n", template);
    close(fd);

    if (stat(template, &sbuf) < 0) {
        if (errno == ENOENT) {
            printf("file doesn't exits\n");
        } else {
            perror("stst()");
            exit(1);
        }
    } else {
        printf("file exists\n");
        unlink(template);
    }

}

int main()
{

    // 使用mkstemp时，必须提供分配的内存用于保存文件名，不能使用没有分配内存的指针
    char good_template[] = "/tmp/dirXXXXXX";
    char *bad_template = "/tmp/dirXXXXXX";

    char *heap_template = malloc(1024);
    if (heap_template == NULL) {
        perror("malloc()");
        exit(1);
    }
    memset(heap_template, 0, sizeof(heap_template));
    strcpy(heap_template, "/tmp/heapuXXXXXX");

    printf("trying to create goof temp file...\n");
    make_temp(good_template);

    printf("trying to create heap temp file ...\n");
    make_temp(heap_template);
    free(heap_template);

    // 这里试图向一个指针中写入文件名，将报错Segmentation fault
    printf("trying to create bad temp file ...\n");
    make_temp(bad_template);

    exit(0);
}