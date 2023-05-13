#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage %s <uid>\n", argv[0]);
    }

    struct passwd *pw;

    pw = getpwuid(atoi(argv[1]));

    printf("%s\n", pw->pw_name);

    exit(1);
}