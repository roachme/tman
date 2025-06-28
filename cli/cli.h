#ifndef LIBTMAN_CLI_H
#define LIBTMAN_CLI_H

#define CMDSIZ          5

#include "help.h"
#include "hook.h"
#include "color.h"
#include "../lib/src/libtman.h"

#define PROGRAM     "tman"

#define LIST_PRJ_UNITS(p) {\
    color_print_char("%c ", (p)->mark, BGRN); \
    color_print_str("%-" xstr(IDSIZ) "s ", (p)->id, BBLU); \
    color_print_str("%s\n", (p)->desc, BWHT); \
}\

/* TODO: add the same function for board list.  */
#define LIST_BRD_UNITS(p) {\
}\

/* TODO: add a plugin output too */
#define LIST_TASK_UNITS(p) {\
    color_print_char("%c ", (p)->mark, BGRN); \
    color_print_str("%-" xstr(IDSIZ) "s ", (p)->id, BMAG); \
    color_print_str("%s\n", (p)->desc, BWHT); \
}\

typedef struct builtin {
    char name[CMDSIZ + 1];
    int setuplvl;
    int (*func)(int argc, char **argv, tman_ctx_t * ctx);
} builtin_t;

int tman_pwd(void);
int tman_pwd_unset(void);
int elog(int status, const char *fmt, ...);
int dlog(int level, const char *fmt, ...);

// TODO: make argv const
int tman_cli_add(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_cfg(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_chk(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_col(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_del(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_prj(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_help(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_init(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_move(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_prev(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_set(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_show(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_plugin(int argc, char **argv, tman_ctx_t * ctx);

#endif
