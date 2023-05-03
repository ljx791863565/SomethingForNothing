#include "err.h"


int main(int argc, char *argv[])
{

#if STDERR_LOG_TEST
    err_ret("test err_ret %s %d", "Warring", 123);
    err_msg("this is err_msg");
    err_quit("this is err_quit");
#endif // STDERR_LOG_TEST

#ifdef SYSLOG_TEST
    log_open("MYLOGTEST", LOG_CONS | LOG_PID, 0);
    log_ret("test err_ret %s %d", "Warring", 123);
    log_msg("this is err_msg");
    log_quit("this is err_quit");
    log_close();
#endif // SYSLOG_TEST
    exit(0);
}