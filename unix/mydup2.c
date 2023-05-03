#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 实现dup2功能，要求不调用fcntl函数，正确处理错误

    1，检查给定的源fd是否有效，且都大于0，

    2，检查目标fd是否超出了系统设定的范围，而这个值在书上是没有着重指出的，

    3，源fd与目标fd是否相等，

    4，利用系统的特性：dup总是返回最小可用的fd，不断重复dup，从而得到一个等于new_fd的fd值
　　再清除掉new_fd之前的临时fd

    5，如果在4）的过程中。如果中途dup失败。则需要在返回失败前，关掉这些临时的fd。
    因此close这些临时fd时，需要区别是创建new_fd成功还是失败了。
 *
*/

static int check_fd(int oldfd, int newfd)
{
    int tab_size = getdtablesize();
    if (oldfd < 0 || newfd < 0) {
        fprintf(stderr, "[check_fd][line:%d]: oldfd or newfd < 0\n", __LINE__);
        return -1;
    } else if (newfd >= tab_size) {
        fprintf(stderr, "[check_fd][line:%d]: newfd out of system limit.\n", __LINE__);
        return -1;
    }

    int index = dup(oldfd);
    if (index == -1) {
        perror("dup()");
        return -1;
    } else {
        close(index);
    }
    return 0;
}

#define INIT_FD -2

static int do_dup2(int oldfd, int newfd)
{
    int index, index_hit = -1, fd_array[newfd];

    for (index = 0; index <= newfd; index++) {
        fd_array[index] = INIT_FD; // initial to INIT_FD
    }

    printf("[do_dup2]: before dup temp fds\n");
    for (index = 0; index <= newfd; index++) {
        fd_array[index] = dup(oldfd);
        printf("[do_dup2]: index: %d, create temp fd: %d\n", index, fd_array[index]);

        if (fd_array[index] == -1) {
            printf("[do_dup2][line:%d]: dup process error!\n", __LINE__);
            break;
        } else if (fd_array[index] == newfd) {
            index_hit = index;
            break;
        }
    }

    // close temp fd
    printf("\n[do_dup2]: to close temp fds\n");

    if (index_hit == -1) { // break for loops with error
        for (index = 0; index < newfd; index++) {
            if ((fd_array[index] == INIT_FD) || (fd_array[index] == -1)) {
                break; // no new temp dup in array
            } else {
                close(fd_array[index]);
                printf("[my_dup2]: index: %d, del temp fd: %d\n", index, fd_array[index]);
            }
        }
        return -1;
    } else {
        // break for loops with hit
        for (index = 0; index < index_hit; index++) {
            close(fd_array[index]);
            printf("[my_dup2]: index: %d, temp fd: %d\n", index, fd_array[index]);
        }
    }
    return newfd;
}

int mydup2(int oldfd, int newfd)
{
    /*
        dup2是原子操作
        如果newfd已经打开，则先将其关闭
        如果oldfd == newfd, 返回newfd而不关闭
        否则newfd 的FD_CLOEXEC文件描述标志被清除，这样newfd在进程执行exec时是打开状态
    */
    int ret = check_fd(oldfd, newfd);
    if (ret == -1) {
        return -1;
    }

    if (oldfd == newfd) {
        return newfd;
    }

    close(newfd);
    ret = do_dup2(oldfd, newfd);
    if (ret == -1) {
        perror("do_dup2()");
        return -1;
    } else {
        return ret;
    }

    return newfd;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <oldfd> <newfd>\n", argv[0]);
        exit(1);
    }

    int oldfd = atoi(argv[1]);
    int newfd = atoi(argv[2]);

    int ret = mydup2(oldfd, newfd);
    if (ret == -1) {
        printf("[mydup2][line:%d] error\n", __LINE__);
        exit(1);
    }

    printf("open newfd [%d] success.\n", newfd);

    exit(0);
}