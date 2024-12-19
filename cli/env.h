#ifndef ENV_H
#define ENV_H

#include "cli.h"

int _env_add(int argc, char **argv, tman_ctx_t *ctx);
int _env_cat(int argc, char **argv, tman_ctx_t *ctx); // under consideration
int _env_del(int argc, char **argv, tman_ctx_t *ctx);
int _env_list(int argc, char **argv, tman_ctx_t *ctx);
int _env_prev(int argc, char **argv, tman_ctx_t *ctx);
int _env_set(int argc, char **argv, tman_ctx_t *ctx);
int _env_use(int argc, char **argv, tman_ctx_t *ctx);

#endif
