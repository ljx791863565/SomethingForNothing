#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main()
{
    int fd = open("/tmp/out", O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (fd < 0) {
        perror("open()");
        exit(1);
    }

    // close(1);
    // dup(fd);
    dup2(fd, 1);

    if (fd != 1) {
        close(fd);
    }

    printf("hello dup test\n");


    /*
     * 现在stdout已经被重定向到文件了， 如果在模块中，最好需要重新打开tty，并将标准输出重定向回stdout
     */
    exit(1);
}