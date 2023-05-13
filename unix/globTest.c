#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>

// #define PAT "/etc/*.conf"
#define PAT "/etc/.*"

int errfunc(const char *epath, int eerrno)
{
    fprintf(stderr, "epath : %s\n", epath);
    fprintf(stderr, "glob error: %s\n", strerror(eerrno));
    return 0;
}

int main(int argc, char *argv[])
{
    glob_t globres;

    int ret = glob(PAT, 0, errfunc, &globres);
    if (ret) {
        perror("glob()");
        exit(1);
    }

    int i = 0;
    for (i = 0; i < globres.gl_pathc; i++) {
        fprintf(stdout, "%s\n", globres.gl_pathv[i]);
    }


    globfree(&globres);
    exit(1);
}