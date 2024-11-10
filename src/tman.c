#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>

#include "env.h"
#include "pgm.h"
#include "tman.h"
#include "core.h"
#include "common.h"
#include "unit.h"
#include "hook.h"
#include "help.h"
#include "osdep.h"

struct tmanstruct tmanfs;

builtin_t builtins[] = {
    /* system commands */
    { .name = "cfg",  .func = &tman_cfg  },
    { .name = "init", .func = &tman_init },

    /* basic commands */
    { .name = "add",  .func = &tman_add  },
    { .name = "del",  .func = &tman_del  },
    { .name = "prev", .func = &tman_prev },
    { .name = "set",  .func = &tman_set  },
    { .name = "sync", .func = &tman_sync },
    { .name = "use",  .func = &tman_use  },

    /* misc commands */
    { .name = "env",  .func = &tman_env  },
    { .name = "pgm",  .func = &tman_pgm  },
    { .name = "move", .func = &tman_move },

    /* info commands */
    { .name = "cat",  .func = &tman_cat  },
    { .name = "help", .func = &tman_help },
    { .name = "list", .func = &tman_list },
    { .name = "ver",  .func = &tman_ver  },
};

builtin_t envcmds[] = {
    { .name = "add",  .func = &_env_add  },
    { .name = "del",  .func = &_env_del  },
    { .name = "list", .func = &_env_list },
    { .name = "prev", .func = &_env_prev },
    { .name = "set",  .func = &_env_set  },
    { .name = "use",  .func = &_env_use  },
};

builtin_t pgmcmds[] = {
    { .name = "chk",  .func = &_pgn_chk  },
    { .name = "list", .func = &_pgn_list },
};

int builtin_size = sizeof(builtins) / sizeof(builtins[0]);
int envcmd_size = sizeof(envcmds) / sizeof(envcmds[0]);
int pgmcmd_size = sizeof(pgmcmds) / sizeof(pgmcmds[0]);


/*

Usage: pgn -b base -e env -i id [OPTIONS]... cmd [ARGS]
Qs:
    1. How can a plugin have access to system stuff?

*/

static int compare(const void *aa, const void *bb)
{
    struct ilist *a = (struct ilist*)aa;
    struct ilist *b = (struct ilist*)bb;
    return (a->col.level - b->col.level);
}

static int pretty_cat(char *env, char *id, char *key)
{
    struct units units;
    memset(&units, 0, sizeof(units));

    if (core_id_cat(&units, env, id) == NULL) {
        return 1;
    }

    for (int i = 0; i < units.builtn.size; ++i) {
        printf("%-7s : %s\n", units.builtn.pair[i].key, units.builtn.pair[i].val);
    }
    for (int i = 0; i < units.plugin.size; ++i) {
        printf("%-7s : %s\n", units.plugin.pair[i].key, units.plugin.pair[i].val);
    }
    return 1;
}

static int pretty_list(char *env)
{
    struct list list;

    memset(&list, 0, sizeof(list));
    if (!core_id_list(&list, env))
        return 1;

    // sort list according to mark level
    // code goes here..
    qsort((void*)list.ilist, list.num, sizeof(list.ilist[0]), compare);

    for (int i = 0; i < list.num; ++i) {
        struct ilist item = list.ilist[i];
        printf("%c %-10s [%s] %s\n", item.col.mark, item.id, item.pgn, item.desc);
    }

    return 1;
}

int tman_cfg(int argc, char **argv)
{
    printf("tman_cfg: under development\n");
    return 1;
}

int tman_init(int argc, char **argv)
{
    char *homedir = getenv("HOME");
    // TODO: should get it from config file
    sprintf(tmanfs.base,  "%s/%s", homedir, "trash/tman");
    sprintf(tmanfs.db,    "%s/%s", tmanfs.base, ".tman");
    sprintf(tmanfs.finit, "%s/%s", tmanfs.db,   "inited");
    sprintf(tmanfs.finit, "%s/%s", tmanfs.db,   "state");
    sprintf(tmanfs.task,  "%s/%s", tmanfs.base, "task");
    sprintf(tmanfs.pgn,   "%s/%s", tmanfs.base, "pgns");

    /* Generic check */
    if (access(tmanfs.finit, F_OK) == 0)
        return 0;

    if (MKDIR(tmanfs.base))
        return elog("could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.task))
        return elog("could not create directory %s", tmanfs.task);
    else if (MKDIR(tmanfs.pgn))
        return elog("could not create directory %s", tmanfs.pgn);
    else if (MKDIR(tmanfs.db))
        return elog("could not create directory %s", tmanfs.db);
    else if (TOUCH(tmanfs.fstate))
        return elog("could not create file %s", tmanfs.fstate);
    else if (TOUCH(tmanfs.finit))
        return elog("could not create init file %s", tmanfs.finit);
    return 0;
}

int tman_pgm(int argc, char **argv)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < pgmcmd_size; ++i)
        if (strncmp(cmd, pgmcmds[i].name, CMDSIZ) == 0)
            return pgmcmds[i].func(argc - 1, argv + 1);

    return elog("no such pgm command '%s'", cmd);
}

int tman_env(int argc, char **argv)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < envcmd_size; ++i)
        if (strncmp(cmd, envcmds[i].name, CMDSIZ) == 0)
            return envcmds[i].func(argc - 1, argv + 1);

    return elog("no such env command '%s'", cmd);
}

int tman_move(int argc, char **argv)
{
    char c;
    char *srcenv = NULL;
    char *dstenv = NULL;

    while ((c = getopt(argc, argv, "s:")) != -1) {
        switch (c) {
            case 's':
                srcenv = optarg; break ;
        }
    }

    if (optind == argc)
        return elog("task id required");
    if (optind + 1 == argc)
        return elog("destination env required");

    dstenv = argv[--argc];
    for (int i = optind; i < argc; ++i) {
        char *id = argv[i];
        core_id_move(id, dstenv, srcenv);
    }

    return 1;
}

int tman_add(int argc, char **argv)
{
    char c;
    char *env = NULL, *id = NULL;
    int o_strict = 0;
    int retcode;

    while ((c = getopt(argc, argv, "e:s")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 's':
                o_strict = 1; break ;
        }
    }

    if (optind == argc)
        return elog("task id required");

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        retcode = core_id_add(env, id);

        if (o_strict && retcode != 0)
            break;
    }
    return retcode == 0 ? core_currdir() : 1;
}

int tman_del(int argc, char **argv)
{
    char c;
    char *env = NULL, *id = NULL;
    int o_strict = 0;
    int o_force = 0;
    int retcode;

    while ((c = getopt(argc, argv, "e:fs")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'f':
                o_force = 1;  break ;
            case 's':
                o_strict = 1; break ;
        }
    }

    if (o_force && o_strict)
        return elog("options -f and -s mutually exclusive");

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        if ((retcode = core_id_del(env, id)) != 0 && o_strict) {
            break;
        }
    }
    if (optind == argc) /* delete current task id */
        retcode = core_id_del(env, id);
    return retcode != 0 ? retcode : core_currdir();
}

int tman_prev(int argc, char **argv)
{
    if (core_id_prev())
        return elog("no previous task is set");
    return core_currdir();
}

/*
Board column structure. Has 4 default values, and lets
a user define his own in config file.
backlog -- previous -- current -- col1 -- col2 -- finished
        |           |           |       |       |
----------------------------------------------------------

* current
^ previous
+ backlog
- finished
*/

/*
List of task's builtin fields. The user can add other fields.
local options = {
    id = id,
    type = "bugfix",
    prio = "mid",
    desc = "autogenerated descriptioN",
    date = tostring(os.date("%Y%m%d")),
}

    tman add
*/

int tman_set(int argc, char **argv)
{
    char c;
    int idx;
    char *env = NULL, *id = NULL;
    struct unit unit = {};

    /*
Options:
    -i      set task id
    -d      set task description
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -s      set task status.
    -t      set task type. Values: [bugfix|hotfix|feature]
    */
    while ((c = getopt(argc, argv, "d:i:p:t:")) != -1) {
        switch (c) {
            case 'i':
                idx = 0;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "id");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 'p':
                idx = 1;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "prio");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 't':
                idx = 2;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "type");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 'd':
                idx = 4;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "desc");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
        }
    }

    if (unit.size == 0)
        return elog("gotta supply one of the options");

    if (optind == argc) {
        core_id_set(env, id, &unit);
        return 1;
    }

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        core_id_set(env, id, &unit);
    }
    return 1;
}

int tman_sync(int argc, char **argv)
{
    if (core_id_sync())
        return elog("no current task is set");
    return core_currdir();
}

int tman_use(int argc, char **argv)
{
    char c;
    char *env = NULL, *id = NULL;

    while ((c = getopt(argc, argv, "e:")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
        }
    }

    if (optind == argc)
        return elog("%s: task id required");

    id = argv[optind];
    if (core_id_use(env, id) != 0)
        return 1;
    return core_currdir();
}

int tman_cat(int argc, char **argv)
{
    char c;
    int retcode;
    int o_strict = 0;
    char *key = NULL;
    char *env = NULL;

    while ((c = getopt(argc, argv, "e:k:s")) != -1) {
        switch (c) {
            case 'e': env = optarg; break;
            case 'k': key = optarg; break;
            case 's': o_strict = 1; break;
        }
    }

    for (int i = optind; i < argc; ++i) {
        char *id = argv[i];
        retcode = pretty_cat(env, id, key);
        if (o_strict && retcode != 0)
            break;
    }

    /* if no arguments passed cat current task (if any) */
    return optind < argc ? retcode : pretty_cat(env, NULL, key);
}

int tman_list(int argc, char **argv)
{
    char c;
    int retcode;

    while ((c = getopt(argc, argv, "a:")) != -1) {
        switch (c) {
            case 'a':
                break ;;
        }
    }

    for (int i = optind; i < argc; ++i) {
        char *env = argv[i];
        retcode = pretty_list(env);
    }

    /* if no arguments passed list current env (if any) */
    return optind < argc ? retcode : pretty_list(NULL);
}

int tman_help(int argc, char **argv)
{
    char c;
    char *cmd;
    char *key = NULL;
    char o_desc = false;

    while ((c = getopt(argc, argv, "dk:")) != -1) {
        switch (c) {
            case 'k':
                key = optarg; break;
        };
    }

    cmd = argv[optind];
    return help_lookup(cmd);
}

int tman_ver(int argc, char **argv)
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 1;
}

int main(int argc, char **argv)
{
    char *cmd = argc > 1 ? argv[1] : "list";

    /* init util for a command. Not all of 'em need initialization */
    if (core_init(cmd))
        return 1;

    for (int i = 0; i < builtin_size; ++i)
        if (strcmp(cmd, builtins[i].name) == 0)
            return builtins[i].func(argc - 1, argv + 1);

    if (isplugin(cmd))
        return plugin(argc, argv);

    return elog("cannot access '%s': no such command or plugin", cmd);
}
