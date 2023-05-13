#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#define MAX_PATH 1024

static int path_noLoop(const char *path)
{
    char *pos = strrchr(path, '/');
    if (pos == NULL) {
        printf("no find\n");
        exit(1);
    }
    if (strcmp(pos+1, ".") == 0 || strcmp(pos+1, "..") == 0) {
        return 0;
    }
    return 1;
}

static int64_t mydu_glob(const char *path)
{
    if (path == NULL) {
        fprintf(stderr, "path is null\n");
        exit(1);
    }
    struct stat statbuf;
    char fullpath[MAX_PATH];
    int64_t sum = 0;
    glob_t globres;
    int i = 0;
    int ret = lstat(path, &statbuf);
    if (ret == -1) {
        perror("lstat()");
        exit(1);
    }
    if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
        return statbuf.st_blocks;
    }

    strncpy(fullpath, path, MAX_PATH);
    strncat(fullpath, "/*", MAX_PATH);
    glob(fullpath, 0, NULL, &globres);
    strncpy(fullpath, path, MAX_PATH);
    strncat(fullpath, "/.*", MAX_PATH);
    glob(fullpath, GLOB_APPEND, NULL, &globres);
    for (i = 0; i < globres.gl_pathc; i++) {
        if (path_noLoop(globres.gl_pathv[i])) {
            sum += mydu_glob(globres.gl_pathv[i]);
        }
    }
    sum += statbuf.st_blocks;

    globfree(&globres);
    return sum;
}

static char fullpath[MAX_PATH];
int pathlen;
// 使用opendir版本没有做权限处理 会遇到无权限dir的情况
static int64_t mydu_dir(const char *path)
{
    if (path == NULL) {
        fprintf(stderr, "path is null\n");
        exit(1);
    }
    struct stat statbuf;
    int64_t sum = 0;
    struct dirent *pdir;
    DIR *dp;

    int ret = lstat(path, &statbuf);
    if (ret == -1) {
        perror("lstat()");
        exit(1);
    }
    if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
        return statbuf.st_blocks;
    }

    strcpy(fullpath, path);
    int n = strlen(fullpath);
    fullpath[n++] = '/';
    fullpath[n] = 0;
    
    // dir can not read
    if ((dp = opendir(fullpath)) == NULL) {
        perror("opendir()");
        exit(1);
    }
    while ((pdir = readdir(dp)) != NULL) {
        if (strcmp(pdir->d_name, ".") == 0 || strcmp(pdir->d_name, "..") == 0) {
            continue;       // ignore
        }
        strcpy(&fullpath[n], pdir->d_name); //FIX: append child dir after '/'

        sum += mydu_dir(fullpath);
    }
    fullpath[n-1] = 0;
    sum += statbuf.st_blocks;
    if (closedir(dp) < 0) {
        perror("closedir()");
        exit(1);
    }
    return sum;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <dir>\n", argv[0]);
        exit(1);
    }

    int64_t blocks1 = mydu_glob(argv[1]) / 2;
    printf("%ld\n", blocks1);

    int64_t blocks2 = mydu_dir(argv[1]) / 2;
    printf("%ld\n", blocks2);

    exit(0);
}