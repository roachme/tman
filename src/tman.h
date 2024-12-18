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

struct tman_id_add_opt {
    int noswitch;   /* Do switch to task (set by default) */
};

struct tman_id_del_opt {
};

struct tman_id_use_opt {
};

struct tman_env_add_opt {
};

struct tman_env_del_opt {
};

struct tman_env_use_opt {
};

/* Generic tman structure used by all(?) API functions.  */
typedef struct tman_context {
    struct list list;
    struct units units;
} ctx;

extern struct tmanstruct tmanfs;

/* Generic util functions.  */
int tman_pwd(void);
int tman_id_free(void);
int tman_env_free(void);
const char *tman_strerror(void);
int tman_init(const char *cmd);
int tman_isplugin(const char *pgn);
int tman_plugin(int argc, char **argv);

/* Task ID functions.  */
int tman_id_prev(void);
int tman_id_sync(void);
int tman_id_col(char *env, char *id, char *tag);
int tman_id_add(char *env, char *id, struct tman_id_add_opt *opt);
int tman_id_del(char *env, char *id, struct tman_id_del_opt *opt);
int tman_id_move(char *id, char *dst, char *src);
int tman_id_set(char *env, char *id, struct unitbin *unitbin);
int tman_id_use(char *env, char *id, struct tman_id_use_opt *opt);
struct units *tman_id_cat(char *env, char *id, struct units *units);
struct list *tman_id_list(struct list *list, char *env);

/* Task environment functions.  */
int tman_env_prev(void);
int tman_env_add(char *env, struct tman_env_add_opt *opt);
int tman_env_del(char *env, struct tman_env_del_opt *opt);
int tman_env_set(char *env);
int tman_env_list(void);
int tman_env_use(char *env);

#endif
