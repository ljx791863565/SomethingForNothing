CFLAGS+=-D_FILE_OFFSET_BITS=64 -D_GUN_SOURCE -Wall -DSYSLOG_TEST=1 -g
CC=gcc
$@: $@.c
	${CC} -o $@ $@.c ${CFLAGS}
