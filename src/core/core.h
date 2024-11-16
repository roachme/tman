#ifndef CORE_H
#define CORE_H

#include "unit.h"
#include "common.h"
#include "../bin/add.h"
#include "../bin/del.h"
#include "../bin/use.h"

#include "../bin/env.h"

#define DESCSIZ         80
#define PGNOUTSCSIZ     80

#define LISTBIN 4
#define LISTPGN 40
#define LISTBINUN 50

#define LSIZE       50
#define LBINNUM     2
#define LBINSIZ     80
#define LPGNSIZ     80

#define NCOLUMNS    10
#define TAGSIZ      4

struct column {
    char mark;
    int level;
    char tag[TAGSIZ + 1];
};

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
struct list *core_id_list(struct list *list, char *env);


int core_init(const char *cmd);
int core_currdir();


/* Task ID stuff */
int core_id_prev(void);
int core_id_sync(void);
int core_id_add (char *id, struct tman_add_opt *opt);
int core_id_del (char *id, struct tman_del_opt *opt);
int core_id_move(char *id, char *dst, char *src);
int core_id_set (char *env, char *id, struct unit *unit);
int core_id_use (char *id, struct tman_use_opt *opt);

struct units *core_id_cat (struct units *units, char *env, char *id);


/* Task environment stuff */
int core_env_prev();
int core_env_add (char *env, struct tman_env_add_opt *opt);
int core_env_del (char *env);
int core_env_set (char *env);
int core_env_use (char *env);

struct help *core_id_help(char *cmd);

#endif
