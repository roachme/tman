#ifndef LIBTMAN_H
#define LIBTMAN_H

#include "unit.h"
#include "errmod.h"
#include "tree.h"

#define CMDSIZ          5
#define TMANPATHSIZE    1024

#define FINITSIZ TMANPATHSIZE + 6 + 10  /* filename + padding */
#define FSTATESIZ TMANPATHSIZE + 6 + 10 /* filename + padding */

struct tmanstruct {
    char db[TMANPATHSIZE + 1];  /* directory for tman metadata */
    char cfg[TMANPATHSIZE + 1]; /* directory for config files */
    char pgn[TMANPATHSIZE + 1]; /* directory for plugin data */
    char base[TMANPATHSIZE + 1];        /* directory for all stuff above */
    char pgnins[TMANPATHSIZE + 1];      /* directory for installed plugins */

    char fcfg[TMANPATHSIZE + 1];        /* system config file */
    char finit[FINITSIZ + 1];   /* file that tells that tman is inited */
    char fstate[FSTATESIZ + 1]; /* file to store tman task state */
};

struct tman_arg;
struct tman_hook;
struct tman_base;
struct tman_unit;
struct tman_option;
struct tman_custom;
struct tman_context;

struct tman_arg {
    char *id;
    char *brd;
    char *prj;
};

struct tman_hook {
    char cmd[10];
    char cmdopt[10];
    char pgntag[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
    struct tman_hook *next;
};

struct tman_base {
    char *pgn;                  /* directory where plugins are installed */
    char *task;                 /* directory where tasks are stored */
};

struct tman_unit {
    char *key;
    char *val;
    struct unit *next;
};

struct tman_option {
    int id_switch;
    int id_generate;
    int prj_switch;
};

struct tman_custom {
    struct tman_hook *hooks;
    // struct tman_prios *prios;
    // struct tman_column *column;
};

enum tman_setuplvl {
    LIBTMAN_SETUPSOFT,
    LIBTMAN_SETUPCHECK,
    LIBTMAN_SETUPHARD,
};

/* Generic tman structure used by all(?) API functions.  */
struct tman_context {
    char id[IDSIZ + 1];
    struct unit *unitbin;
    struct unit *unitpgn;
    struct tree *ids;
    struct tree *prjs;
};

extern struct tmanstruct tmanfs;

/* Core functions.  */
int tman_setup(int setuplvl);
struct tman_context *tman_init(struct tman_base *structure);
struct tman_context *tman_deinit(struct tman_context *ctx);

/* Core util functions.  */
int tman_mkfs(void);
const char *tman_strerror(void);

/* Input irguments functions.  */
int tman_check_arg_id(struct tman_arg *args);
int tman_check_arg_brd(struct tman_arg *args);
int tman_check_arg_prj(struct tman_arg *args);

/* Task ID functions.  */
int tman_task_add(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options);
int tman_task_col(struct tman_context *ctx, struct tman_arg *args, char *tag,
                  struct tman_option *options);
int tman_task_del(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options);
int tman_task_list(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options);
int tman_task_move(struct tman_context *ctx, struct tman_arg *src,
                   struct tman_arg *dst);
int tman_task_prev(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options);
int tman_task_set(struct tman_context *ctx, struct tman_arg *args,
                  struct unit *unitbin, struct tman_option *options);
int tman_task_show(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options);
int tman_task_sync(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options);

struct unit *tman_unit_add(struct unit *head, char *key, char *val);
void *tman_unit_free(struct tman_context *ctx, struct tman_arg *args,
                     struct tman_option *options);

/* Task project functions.  */
int tman_prj_add(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options);
int tman_prj_del(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options);
int tman_prj_list(struct tman_context *ctx, struct tman_option *options);
int tman_prj_prev(struct tman_context *ctx, struct tman_option *options);
int tman_prj_rename(struct tman_context *ctx, struct tman_arg *src,
                    struct tman_arg *dst);
int tman_prj_set(struct tman_context *ctx, struct tman_arg *args,
                 struct unit *unitbin, struct tman_option *options);
int tman_prj_show(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options);
int tman_prj_sync(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options);
char *tman_prj_getcurr(struct tman_context *ctx);

/* Task plugin functions.  */
int tman_ispgn(char *pgndir, const char *pgname);
int tman_pgnexec(struct tman_context *ctx, struct tman_arg *args, char *pgname,
                 char *pgncmd, struct tman_option *options);

int tman_hook_action(struct tman_context *ctx, struct tman_hook *hooks,
                     struct tman_arg *args, char *cmd);
struct unit *tman_hook_show(struct tman_context *ctx, struct tman_hook *hooks,
                            struct tman_arg *args, char *cmd);

int tman_hook_action_free(struct tman_context *ctx, struct tman_arg *args,
                          char *cmd);
struct unit *tman_hook_show_free(struct tman_context *ctx,
                                 struct tman_arg *args);

#endif
