#ifndef TMAN_H
#define TMAN_H

#include "unit.h"
#include "errmod.h"
#include "tree.h"

#define CMDSIZ          5
#define TMANPATHSIZE    1024

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
    struct units units;
    struct tree *ids;
    struct tree *prjs;
} tman_ctx_t;

struct tman_id_add_opt {
    int doswitch;   /* Switch to newly created task */
    int dogenerate; /* Generate task ID */
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
    int doswitch;   /* Switch to newly created task */
};

struct tman_id_list_opt {
};



struct tman_pgn_opt {
};


struct tman_prj_add_opt {
};

struct tman_prj_del_opt {
};

struct tman_prj_list_opt {
};

struct tman_prj_prev_opt {
};

struct tman_prj_set_opt {
};

struct tman_prj_use_opt {
};

extern struct tmanstruct tmanfs;

/* Core functions.  */
int tman_setup(int setuplvl);
struct tman_context *tman_init(void);
int tman_deinit(struct tman_context *ctx);

/* Core util functions.  */
int tman_pwd(void);
int tman_mkfs(void);
const char *tman_strerror(void);

/* Task ID functions.  */
int tman_id_add (tman_ctx_t *ctx, char *prj, char *id, struct tman_id_add_opt *opt);
int tman_id_cat (tman_ctx_t *ctx, char *prj, char *id, struct tman_id_cat_opt *opt);
int tman_id_col (tman_ctx_t *ctx, char *prj, char *id, char *tag, struct tman_id_col_opt *opt);
int tman_id_del (tman_ctx_t *ctx, char *prj, char *id, struct tman_id_del_opt *opt);
int tman_id_list(tman_ctx_t *ctx, char *prj, struct tman_id_list_opt *opt);
int tman_id_link(tman_ctx_t *ctx, char *prj, struct tman_id_list_opt *opt);
int tman_id_move(tman_ctx_t *ctx, char *srcprj, char *dstprj, char *srcid, char *dstid);
int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt);
int tman_id_set (tman_ctx_t *ctx, char *prj, char *id, struct unit *unitbin, struct tman_id_set_opt *opt);
int tman_id_sync(tman_ctx_t *ctx, char *prj, char *id, struct tman_id_sync_opt *opt);
int tman_id_use (tman_ctx_t *ctx, char *prj, char *id, struct tman_id_use_opt *opt);
char *tman_id_getcurr(tman_ctx_t *ctx, char *prj);
char *tman_id_getprev(tman_ctx_t *ctx, char *prj);

/* Task project functions.  */
int tman_prj_add (tman_ctx_t *ctx, char *prj, struct tman_prj_add_opt *opt);
int tman_prj_del (tman_ctx_t *ctx, char *prj, struct tman_prj_del_opt *opt);
int tman_prj_list(tman_ctx_t *ctx, struct tman_prj_list_opt *opt);
int tman_prj_prev(tman_ctx_t *ctx, struct tman_prj_prev_opt *opt);
int tman_prj_set (tman_ctx_t *ctx, char *prj, struct tman_prj_set_opt *opt);
int tman_prj_use (tman_ctx_t *ctx, char *prj, struct tman_prj_use_opt *opt);
char *tman_prj_getcurr(tman_ctx_t *ctx);
char *tman_prj_getprev(tman_ctx_t *ctx);

/* Task plugin functions.  */
int tman_ispgn(const char *pgn);
int tman_pgnexec(tman_ctx_t *ctx, char *prj, char *id, char *pgname, char *pgncmd, struct tman_pgn_opt *opt);

#endif
