#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

int main()
{
    char buf[BUFSIZE] = {0};

    int ret = snprintf(buf, BUFSIZE, "%s", "hrlloworld");

    fprintf(stdout, "buf [%d][%s]\n", ret, buf);

    ret = snprintf(buf, BUFSIZE, "%d", -12800000);

    fprintf(stdout, "buf [%d][%s]\n", ret, buf);

    ret = snprintf(buf, BUFSIZE, "%+d", -12800000);

    fprintf(stdout, "buf [%d][%s]\n", ret, buf);
    exit(0);
}