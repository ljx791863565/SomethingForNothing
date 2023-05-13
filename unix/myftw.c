#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>

#define FTW_F   1
#define FTW_D   2
#define FTW_DNR 3
#define FTW_NS  4

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntotal;
static char *fullpath;
static size_t pathlen;

static char *path_alloc(size_t *pathlen)
{
    char *pathname = malloc(PATH_MAX +1);
    if (pathname == NULL) {
        perror("malloc()");
        *pathlen = 0;
        return NULL;
    }
    *pathlen = PATH_MAX+1;
    return pathname;
}

static int myfunc(const char *pathname, const struct stat *pstat, int type)
{
    switch ((type)) {
    case FTW_F:
        switch (pstat->st_mode & S_IFMT) {
        case S_IFREG:
            nreg++;
            break;
        case S_IFBLK:
            nblk++;
            break;
        case S_IFCHR:
            nchr++;
            break;
        case S_IFIFO:
            nfifo++;
            break;
        case S_IFLNK:
            nslink++;
            break;
        case S_IFSOCK:
            nsock++;
            break;
        case S_IFDIR:
            fprintf(stderr, "dir should have type FTW_D\n");
            return -1;
        default:
            break;
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        fprintf(stderr, "can't read dir %s\n", pathname);
        return -1;
    case FTW_NS:
        fprintf(stderr, "stat error for %s\n", pathname);
        return -1;
    default:
        fprintf(stderr, "unknown type %d for pathname %s\n", type, pathname);
        break;
    }
    return 0;
}
typedef int (*MyFunc)(const char *, const struct stat *, int);

static int dopath(MyFunc func)
{
    struct stat statbuf;
    struct dirent *pdir;
    DIR *dp;
    int ret, n;
    if (lstat(fullpath, &statbuf) < 0) {
        perror("lstat()");
        return(func(fullpath, &statbuf, FTW_NS));
    }
    if (S_ISDIR(statbuf.st_mode) == 0) {
        // not dir
        return (func(fullpath, &statbuf, FTW_F));
    }
    // is dir
    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0) {
        printf("is dir %s\n", fullpath);
        return ret;
    }
    n = strlen(fullpath);
    if (n + NAME_MAX +2 > pathlen) {
        pathlen *= 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL) {
            perror("realloc()");
            exit(1);            // FIX: return -1;???
        }
    }
    fullpath[n++] = '/';
    fullpath[n] = 0;            // why 0? is '\0' ?
    
    // dir can not read
    if ((dp = opendir(fullpath)) == NULL) {
        perror("opendir()");
        return (func(fullpath, &statbuf, FTW_DNR));
    }
    while ((pdir = readdir(dp)) != NULL) {
        if (strcmp(pdir->d_name, ".") == 0 || strcmp(pdir->d_name, "..") == 0) {
            continue;       // ignore
        }
        strcpy(&fullpath[n], pdir->d_name); //FIX: append child dir after '/'
        if ((ret = dopath(func)) != 0) {
            printf("ret = -1\n");
            break;          // recursive to last dir, break
        }
    }
    fullpath[n-1] = 0;        // ???
    if (closedir(dp) < 0) {
        perror("closedir()");
        exit(1);
    }

    return ret;
}


static int myftw(const char *pathname, MyFunc func)
{
    fullpath = path_alloc(&pathlen);
    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) *2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL) {
            perror("realloc()");
            exit(1);            // FIX: return -1;???
        }
    }
    strncpy(fullpath, pathname, strlen(pathname));
    printf("[myftw][%ld][%s]\n",pathlen, fullpath);
    return (dopath(func));
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage %s <dir>\n", argv[0]);
        exit(1);
    }

    int ret = myftw(argv[1], myfunc);
    if (ret < 0) {
        perror("myftw()");
        exit(1);
    }

    ntotal = nreg + ndir + nchr + nblk + nfifo + nslink + nsock;
    if (ntotal == 0) {
        printf("ntotal = 0\n");
        ntotal = 1;
    }
    printf("regular file        = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntotal);
    printf("directories         = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntotal);
    printf("block special       = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntotal);
    printf("char special        = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntotal);
    printf("FIFOs               = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntotal);
    printf("symbolic links      = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntotal);
    printf("sockets             = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntotal);

    exit(0);
}