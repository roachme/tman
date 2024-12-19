#ifndef TMAN_H
#define TMAN_H

#include "unit.h"
#include "common.h"
#include "column.h"
#include "osdep.h"
#include "errmod.h"

#define LISTBIN         4
#define CMDSIZ          5
#define LISTPGN         40
#define LISTBINUN       50
#define TMANPATHSIZE    1024

#define LSIZE           50
#define LBINNUM         2
#define LBINSIZ         80
#define LPGNSIZ         80

struct ilist {
    char id[IDSIZ + 1];
    char pgn[LPGNSIZ + 1];
    char desc[DESCSIZ + 1];
    struct column col;
};

struct list {
    size_t num;
    size_t capac;
    struct ilist ilist[LSIZE];
};

struct tmanstruct {
    char db[TMANPATHSIZE + 1];          /* directory for tman metadata */
    char cfg[TMANPATHSIZE + 1];         /* directory for config files */
    char pgn[TMANPATHSIZE + 1];         /* directory for plugin data */
    char base[TMANPATHSIZE + 1];        /* directory for all stuff above */
    char pgnins[TMANPATHSIZE + 1];      /* directory for installed plugins */

    char fcfg[TMANPATHSIZE + 1];        /* system config file */
    char finit[TMANPATHSIZE + 1];       /* file that tells that tman is inited */
    char fstate[TMANPATHSIZE + 1];      /* file to store tman task state */
};

enum tman_setuplvl {
    TMAN_SETUPSOFT,
    TMAN_SETUPCHECK,
    TMAN_SETUPHARD,
};

/* Generic tman structure used by all(?) API functions.  */
typedef struct tman_context {
    struct list list;
    struct units units;
} tman_ctx_t;




struct tman_id_add_opt {
    int noswitch;   /* Do switch to task (set by default) */
};

struct tman_id_cat_opt {
};

struct tman_id_col_opt {
};

struct tman_id_del_opt {
};

struct tman_id_use_opt {
};

struct tman_id_set_opt {
};

struct tman_id_prev_opt {
};

struct tman_id_sync_opt {
};

struct tman_id_list_opt {
};



struct tman_pgn_opt {
};


struct tman_env_add_opt {
};

struct tman_env_del_opt {
};

struct tman_env_list_opt {
};

struct tman_env_prev_opt {
};

struct tman_env_set_opt {
};

struct tman_env_use_opt {
};

extern struct tmanstruct tmanfs;

/* Generic util functions.  */
struct tman_context *tman_init(void);
int tman_setup(int setuplvl);
int tman_deinit(struct tman_context *ctx);

int tman_pwd(void);
int tman_id_free(void);
int tman_env_free(void);
const char *tman_strerror(void);
int tman_mkfs(void);

int tman_isplugin(const char *pgn);
int tman_plugin(tman_ctx_t *ctx, int argc, char **argv, struct tman_pgn_opt *opt);

/* Task ID functions.  */
// TODO: for tman_id_list() and tman_id_cat() use ctx
int tman_id_add (tman_ctx_t *ctx, char *env, char *id, struct tman_id_add_opt *opt);
int tman_id_cat (tman_ctx_t *ctx, char *env, char *id, struct tman_id_cat_opt *opt);
int tman_id_col (tman_ctx_t *ctx, char *env, char *id, char *tag, struct tman_id_col_opt *opt);
int tman_id_del (tman_ctx_t *ctx, char *env, char *id, struct tman_id_del_opt *opt);
struct list *tman_id_list(tman_ctx_t *ctx, struct list *list, char *env, struct tman_id_list_opt *opt);
int tman_id_move(tman_ctx_t *ctx, char *id, char *dst, char *src);
int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt);
int tman_id_set (tman_ctx_t *ctx, char *env, char *id, struct unitbin *unitbin);
int tman_id_sync(tman_ctx_t *ctx, struct tman_id_sync_opt *opt);
int tman_id_use (tman_ctx_t *ctx, char *env, char *id, struct tman_id_use_opt *opt);

/* Task environment functions.  */
int tman_env_add (tman_ctx_t *ctx, char *env, struct tman_env_add_opt *opt);
int tman_env_del (tman_ctx_t *ctx, char *env, struct tman_env_del_opt *opt);
int tman_env_list(tman_ctx_t *ctx, struct tman_env_list_opt *opt);
int tman_env_prev(tman_ctx_t *ctx, struct tman_env_prev_opt *opt);
int tman_env_set (tman_ctx_t *ctx, char *env, struct tman_env_set_opt *opt);
int tman_env_use (tman_ctx_t *ctx, char *env, struct tman_env_use_opt *opt);

#endif
