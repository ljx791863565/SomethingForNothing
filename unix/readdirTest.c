#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <dir>\n", argv[0]);
        exit(1);
    }


    DIR *dp;
    dp = opendir(argv[1]);
    if (dp == NULL) {
        perror("opendir()");
        exit(1);
    }

    struct dirent *pdir;
    while ((pdir = readdir(dp)) != NULL) {
        printf("%s\n", pdir->d_name);
    }

    closedir(dp);


    exit(1);
}