#ifndef TMAN_COMMON_H
#define TMAN_COMMON_H

/* TODO: use stdbool or add prefix or move it to cli part.  */
#define TRUE        1
#define FALSE       0

int elog(int status, const char *fmt, ...);
int dlog(int level, const char *fmt, ...);

#endif
