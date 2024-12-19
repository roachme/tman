#ifndef TMAN_CLI_H
#define TMAN_CLI_H

#define CMDSIZ          5

#include "help.h"
#include "../src/tman.h"

typedef struct builtin {
    char name[CMDSIZ + 1];
    int setuplvl;
    int (*func)(int argc, char **argv, tman_ctx_t *ctx);
} builtin_t;

// TODO: make argv const
int tman_cli_add(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_cat(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_cfg(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_chk(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_col(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_del(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_env(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_help(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_init(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_list(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_move(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_pgm(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_prev(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_set(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_sync(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_use(int argc, char **argv, tman_ctx_t *ctx);
int tman_cli_ver(int argc, char **argv, tman_ctx_t *ctx);

#endif
