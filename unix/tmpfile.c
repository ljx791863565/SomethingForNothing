#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 128
int main()
{
    char buf[BUFSIZE] = {0};

    // fprintf(stdout, "%s\n", tmpnam(NULL));

    fprintf(stdout, "%d\n", mkstemp("/tmp/fileXXXXXX"));


    exit(0);
}