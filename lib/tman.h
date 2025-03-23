#ifndef TMAN_H
#define TMAN_H

#include "unit.h"
#include "errmod.h"
#include "tree.h"

#define CMDSIZ          5
#define TMANPATHSIZE    1024

struct tmanstruct {
    char db[TMANPATHSIZE + 1];  /* directory for tman metadata */
    char cfg[TMANPATHSIZE + 1]; /* directory for config files */
    char pgn[TMANPATHSIZE + 1]; /* directory for plugin data */
    char base[TMANPATHSIZE + 1];        /* directory for all stuff above */
    char pgnins[TMANPATHSIZE + 1];      /* directory for installed plugins */

    char fcfg[TMANPATHSIZE + 1];        /* system config file */
    char finit[TMANPATHSIZE + 1];       /* file that tells that tman is inited */
    char fstate[TMANPATHSIZE + 1];      /* file to store tman task state */
};

struct tman_args;
struct tman_option;
struct tman_context;

struct tman_args {
    char *id;
    char *brd;
    char *prj;
};

enum tman_setuplvl {
    TMAN_SETUPSOFT,
    TMAN_SETUPCHECK,
    TMAN_SETUPHARD,
};

/* Generic tman structure used by all(?) API functions.  */
struct tman_context {
    struct units units;
    struct tree *ids;
    struct tree *prjs;
};

struct tman_id_add_opt {
    int doswitch;               /* Switch task */
    int dogenerate;             /* Generate task ID */
};

struct tman_id_show_opt {
};

struct tman_id_col_opt {
};

struct tman_id_del_opt {
};

struct tman_id_set_opt {
};

struct tman_id_prev_opt {
};

struct tman_id_sync_opt {
    int doswitch;               /* Switch task */
};

struct tman_id_list_opt {
};

struct tman_pgn_opt {
};

struct tman_prj_add_opt {
    int doswitch;               /* Switch to newly created project  */
};

struct tman_prj_del_opt {
};

struct tman_prj_list_opt {
};

struct tman_prj_prev_opt {
};

struct tman_prj_set_opt {
};

struct tman_prj_sync_opt {
    int doswitch;               /* Switch to project  */
};

extern struct tmanstruct tmanfs;

/* Core functions.  */
int tman_setup(int setuplvl);
struct tman_context *tman_init(void);
struct tman_context *tman_deinit(struct tman_context *ctx);

/* Core util functions.  */
int tman_pwd(void);
int tman_mkfs(void);
const char *tman_strerror(void);

/* Input irguments functions.  */
int tman_get_args(struct tman_args *args);
int tman_check_arg_id(struct tman_args *args);
int tman_check_arg_prj(struct tman_args *args);

/* Task ID functions.  */
int tman_id_add2(struct tman_context *ctx, struct tman_args *args,
                 struct tman_option *opt);
int tman_id_add(struct tman_context *ctx, struct tman_args *args,
                struct tman_id_add_opt *opt);
int tman_id_col(struct tman_context *ctx, struct tman_args *args, char *tag,
                struct tman_id_col_opt *opt);
int tman_id_del(struct tman_context *ctx, struct tman_args *args,
                struct tman_id_del_opt *opt);
int tman_id_find_by_desc(struct tman_context *ctx, struct tman_args *args,
                         char *descpatt);
int tman_id_list(struct tman_context *ctx, struct tman_args *args,
                 struct tman_id_list_opt *opt);
int tman_id_link(struct tman_context *ctx, struct tman_args *args,
                 struct tman_id_list_opt *opt);
int tman_id_move(struct tman_context *ctx, char *srcprj, char *dstprj,
                 char *srcid, char *dstid);
int tman_id_prev(struct tman_context *ctx, struct tman_args *args,
                 struct tman_id_prev_opt *opt);
int tman_id_set(struct tman_context *ctx, struct tman_args *args,
                struct unit *unitbin, struct tman_id_set_opt *opt);
int tman_id_show(struct tman_context *ctx, struct tman_args *args,
                 struct tman_id_show_opt *opt);
int tman_id_sync(struct tman_context *ctx, struct tman_args *args,
                 struct tman_id_sync_opt *opt);
char *tman_id_getcurr(struct tman_context *ctx, struct tman_args *args);
char *tman_id_getprev(struct tman_context *ctx, struct tman_args *args);

/* Task project functions.  */
int tman_prj_add(struct tman_context *ctx, struct tman_args *args,
                 struct tman_prj_add_opt *opt);
int tman_prj_del(struct tman_context *ctx, struct tman_args *args,
                 struct tman_prj_del_opt *opt);
int tman_prj_list(struct tman_context *ctx, struct tman_prj_list_opt *opt);
int tman_prj_prev(struct tman_context *ctx, struct tman_prj_prev_opt *opt);
int tman_prj_set(struct tman_context *ctx, struct tman_args *args,
                 struct tman_prj_set_opt *opt);
int tman_prj_sync(struct tman_context *ctx, struct tman_args *args,
                  struct tman_prj_sync_opt *opt);
char *tman_prj_getcurr(struct tman_context *ctx);
char *tman_prj_getprev(struct tman_context *ctx);

/* Task plugin functions.  */
int tman_ispgn(const char *pgn);
int tman_pgnexec(struct tman_context *ctx, struct tman_args *args, char *pgname,
                 char *pgncmd, struct tman_pgn_opt *opt);

#endif
