#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE 64
ssize_t mygetline(char **lineptr, size_t *n, FILE *stream)
{
	if (*lineptr == NULL || *n == 0) {
		*lineptr = malloc(LINE);
		*n = LINE;
		if (*lineptr == NULL) {
			perror("malloc()");
			return -1;
		}
	}

	int ch;
	ssize_t count = 0;
	while ((ch = fgetc(stream)) != '\n') {
		if (ch == EOF) {
			return -1;
		}
		
		if (count < *n-2) {
			*(*lineptr + count++) = ch;
		} else {
			*n += LINE;
			*lineptr = realloc(*lineptr, *n);
			if (*lineptr == NULL) {
				perror("realloc()");
				return -1;
			}
			*(*lineptr + count++) = ch;
		}
	}
	*(*lineptr + count++) = '\n';
	*(*lineptr + count) = '\0';
	return count;

}
void mygetline_free(char *lineptr)
{
	if (lineptr != NULL) {
		free(lineptr);
		lineptr = NULL;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file_name>", argv[0]);
		exit(1);
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("fopen()");
		exit(1);
	}
	
	char *linebuf = NULL;
	size_t linelen = 0;

	while (1) {
		if (mygetline(&linebuf, &linelen, fp) < 0) {
			break;
		}
		fprintf(stdout, "%lu\n", strlen(linebuf));
		fprintf(stdout, "%lu\n", linelen);
	}

	mygetline_free(linebuf);
	fclose(fp);
	exit(0);
}
