#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 8192

/*
 * 测试BUFSZIE大小对性能影响  copy一个3.5G大小文件，测试耗时
 *  BUFSIZE     user CPU(s)     sys CPU(s)      time(s)     count
 *  4           726             1118.597        1844.597    917436332
 *  8           258.226         668.632         927.1       458718166
 *  16          129.094         337.120         446.389     229359083
 *  32          95.634          141.497         237.299     114679542
 *  64          33.125          87.452          121.07      57339771
 *  128         15.746          44.726          60.881      28669886
 *  256         7.781           23.002          31.128      14334943
 *  512         3.79            12.576          17.475      7167472
 *  1024        1.933           7.201           10.407      3583736
 *  2048        1.002           4.510           6.657       1791868
 *  4096        0.489           3.058           4.705       895934
 *  8192        0.254           2.662           4.147       447967
 *  16384       0.141           2.438           4.164       223984
 *  32768       0.108           2.354           3.892       111992
 *  65536       0.033           2.421           3.757       55996
 *  131072      0.029           2.605           4.024       27998
 *  262144      0.009           2.421           3.818       13999
 *  524288      0.001           2.636           3.852       7000
 *  1048576     0.005           2.266           3.666       3500
 *  2097152     0.005           2.355           3.731       1750
 *  4194304     0.004           2.292           3.936       875
 *  8388608 段错误  POSIX message queues :819200bytes = 819200*8 = 6553600  < 8388608
 *  比较合适大小为8192
*/

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src file> <dest file>\n", argv[0]);
        exit(1);
    }

    int srcFd = open(argv[1], O_RDONLY);
    if (srcFd < 0) {
        perror("open()");
        exit(1);
    }
    int destFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (destFd < 0) {
        perror("open()");
        close(srcFd);
        exit(1);
    }

    char buf[BUFSIZE] = {0};
    int len = 0, ret = 0, pos;
    long long count = 0;

    while (1) {
        len = read(srcFd, buf, BUFSIZE);
        if (len  < 0) {
            perror("read()");
            break;
        }
        if (len == 0) {
            break;
        }
        count++;
        pos = 0;
        while (len > 0) {
            ret = write(destFd, buf+pos, len);
            if (ret < 0) {
                perror("write()");
                // exit(1);
                goto RETURN;
            }
            pos += ret;
            len -= ret;
        }
    }
    printf("count = %lld, BUFSIZE = %d\n", count, BUFSIZE);
RETURN:
    close(destFd);
    close(srcFd);
    exit(0);
}