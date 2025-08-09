#ifndef LIBTMAN_CLI_H
#define LIBTMAN_CLI_H

#include <stdio.h>
#include <getopt.h>

#include "aux/hook.h"
#include "aux/color.h"
#include "../lib/src/libtman.h"

#define PROGRAM     "tman"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define xstr(s) str(s)
#define str(s) #s

#define IDSIZ           10
#define BRDSIZ          10
#define PRJSIZ          10
#define CMDSIZ          10
#define COLSIZ          7

#define FIRST_COLUMN     0
#define LAST_COLUMN      100

#define xstr(s)     str(s)
#define str(s)      #s

#define IDLIMIT     9999999
#define IDFMT       "%0" xstr(IDSIZ) "u"
#define PADDING_UNIT     6

/* TODO: add a plugin output too */
#define LIST_PROJECT_UNITS(_mark, _id, _desc) {\
    color_print_str("%-" xstr(COLSIZ) "s ", (_mark), BGRN); \
    color_print_str("%-" xstr(IDSIZ) "s ", (_id), BMAG); \
    color_print_str("%s\n", (_desc), BWHT); \
}\

/* TODO: add a plugin output too */
#define LIST_BOARD_UNITS(_mark, _id, _desc) {\
    color_print_str("%-" xstr(COLSIZ) "s ", (_mark), BGRN); \
    color_print_str("%-" xstr(IDSIZ) "s ", (_id), BMAG); \
    color_print_str("%s\n", (_desc), BWHT); \
}\

/* TODO: add a plugin output too */
#define LIST_TASK_UNITS(_mark, _id, _desc) {\
    color_print_str("%-" xstr(COLSIZ) "s ", (_mark), BGRN); \
    color_print_str("%-" xstr(IDSIZ) "s ", (_id), BMAG); \
    color_print_str("%s\n", (_desc), BWHT); \
}\

#define CTX_INIT { .column = NULL, .units = NULL, .list = NULL }

enum tman_setup_level {
    TMAN_PAGER,
    TMAN_SETUP_SOFT,
    TMAN_SETUP_HARD,
};

typedef struct builtin {
    const char *name;
    int (*func)(int argc, char **argv, tman_ctx_t * ctx);
    unsigned int option;
} builtin_t;

typedef struct column {
    int prio;
    char *mark;
    char *name;
} column_t;

extern char *unitkeys[];
extern unsigned int nunitkey;

extern column_t builtin_columns[];
extern unsigned int nbuiltin_column;

int is_valid_length(const char *obj, int len);
int check_arg_project(tman_arg_t * args, const char *errfmt, int quiet);
int check_arg_board(tman_arg_t * args, const char *errfmt, int quiet);
int check_arg_task(tman_arg_t * args, const char *errfmt, int quiet);

BOOL column_exist(const char *colname);
tman_unit_t *generate_column(char *colname);

int help_list_commands(void);
int help_usage(const char *cmd);
int help_lookup(const char *cmd);

int tman_pwd_unset(void);
int tman_pwd_task(tman_arg_t * args);
int tman_pwd_board(tman_arg_t * args);
int tman_pwd_project(tman_arg_t * args);
int elog(int status, const char *fmt, ...);
int dlog(int level, const char *fmt, ...);

// NOTE: maybe use 'prefix' like in git?
// int cmd_add(int argc, const char **argv, const char *prefix, struct repository *repo);

// TODO: make argv const
int tman_cli_add(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_board(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_column(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_del(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_help(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_init(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_move(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_plugin(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_prev(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_project(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_set(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_show(int argc, char **argv, tman_ctx_t * ctx);
int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx);

#endif
