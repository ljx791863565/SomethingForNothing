#ifndef __APUE_ERR_H__
#define __APUE_ERR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#ifndef BUFSIZE
#define BUFSIZE 8192
#endif // BUFSIZE

int log_to_stderr = 1;

static void err_doit(int errflags, int error, const char *fmt, va_list ap)
{
    char buf[BUFSIZE];
    vsnprintf(buf, BUFSIZE-1, fmt, ap);
    if (errflags) {
        snprintf(buf+strlen(buf), BUFSIZE-strlen(buf)-1, ": %s", strerror(error));
    }
    strcat(buf, "\n");
    fflush(stdout);
    fputs(buf, stderr);
    fflush(NULL);
}

void err_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
}

void err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}

void err_cont(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, error, fmt, ap);
    va_end(ap);
}

void err_exit(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, error, fmt, ap);
    va_end(ap);
    exit(1);
}

void err_dump(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    abort();
    exit(1);
}

void err_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

static void sys_err_doit(int errflags, int error, int priority, const char *fmt, va_list ap)
{
    char buf[BUFSIZE];
    vsnprintf(buf, BUFSIZE-1, fmt, ap);
    if (errflags) {
        snprintf(buf+strlen(buf), BUFSIZE-strlen(buf)-1, ": %s", strerror(error));
    }
    strcat(buf, "\n");
    if (log_to_stderr) {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(NULL);
    } else {
        syslog(priority, "%s", buf);
    }
}

void log_open(const char *ident, int option, int facility)
{
    if (log_to_stderr == 0) {
        openlog(ident, option, facility);
    } 
}

void log_close()
{
    if (log_to_stderr == 0) {
        closelog();
    }
}

void log_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sys_err_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
}

void log_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sys_err_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

void log_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sys_err_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
}

void log_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sys_err_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

void log_exit(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    sys_err_doit(1, error, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

#endif // __APUE_ERR_H__