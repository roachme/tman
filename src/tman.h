#ifndef TMAN_H
#define TMAN_H

#include "unit.h"
#include "common.h"
#include "column.h"
#include "osdep.h"

// TODO: get rid of 'em. Use generic structure for options and
// return value in ubus in OpenWrt does.
#include "../cli/add.h"
#include "../cli/del.h"
#include "../cli/use.h"
#include "../cli/env.h"

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

extern struct tmanstruct tmanfs;

/* Generic util functions.  */
int core_pwd(void);
int core_init(const char *cmd);
int core_isplugin(const char *pgn);
int core_plugin_exec(int argc, char **argv);
struct help *core_id_help(const char *cmd);

/* Task ID functions.  */
int core_id_prev(void);
int core_id_sync(void);
int core_id_movecol(char *env, char *id, char *tag);
int core_id_add(char *env, char *id, struct tman_add_opt *opt);
int core_id_del(char *env, char *id, struct tman_del_opt *opt);
int core_id_move(char *id, char *dst, char *src);
int core_id_set(char *env, char *id, struct unitbin *unitbin);
int core_id_use(char *env, char *id, struct tman_use_opt *opt);
struct units *core_id_cat(char *env, char *id, struct units *units);
struct list *core_id_list(struct list *list, char *env);

/* Task environment functions.  */
int core_env_prev(void);
int core_env_add(char *env, struct tman_env_add_opt *opt);
int core_env_del(char *env, struct tman_env_del_opt *opt);
int core_env_set(char *env);
int core_env_use(char *env);

#endif
