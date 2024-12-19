#include <string.h>
#include <dirent.h>

#include "env.h"
#include "cli.h"

static const builtin_t envcmds[] = {
    { .name = "add",  .func = &_env_add  },
    { .name = "cat",  .func = &_env_cat  }, // under consideration
    { .name = "del",  .func = &_env_del  },
    { .name = "list", .func = &_env_list },
    { .name = "prev", .func = &_env_prev },
    { .name = "set",  .func = &_env_set  },
    { .name = "use",  .func = &_env_use  },
};

// TODO: Find a good error message in case option fails.  */
int _env_add(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int force, status;
    struct tman_env_add_opt opt;

    force = FALSE;
    while ((c = getopt(argc, argv, ":f")) != -1) {
        switch (c) {
            case 'f':
                force = 1; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (optind == argc)
        return elog(1, "task env required");

    for (int i = optind; i < argc; ++i)
        status = tman_env_add(ctx, argv[i], &opt);
    return status == TMAN_OK ? tman_pwd() : 1;
}

// roach: maybe it'll be useful
int _env_cat(int argc, char **argv, tman_ctx_t *ctx)
{
    return 0;
}

int _env_del(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int force, status;
    int showpath = FALSE;
    char *old_cenv = column_getcenv();
    //char *old_penv = column_getpenv();
    struct tman_env_del_opt opt;

    force = FALSE;
    while ((c = getopt(argc, argv, ":f")) != -1) {
        switch (c) {
            case 'f':
                force = 1; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    for (int i = optind; i < argc; ++i)
        status = tman_env_del(ctx, argv[i], &opt);
    if (optind == argc) /* delete current task id */
        status = tman_env_del(ctx, NULL, &opt);

    // TODO: update current directory if current env got deleted.
    if (strcpy(old_cenv, column_getcenv())) {
        showpath = TRUE;
    }

    return status == TMAN_OK && showpath == TRUE ? tman_pwd() : 1;
}

int _env_list(int argc, char **argv, tman_ctx_t *ctx)
{
    char mark = '+';
    struct dirent *ent;
    char *cenv = column_getcenv();
    char *penv = column_getpenv();
    DIR *edir = opendir(tmanfs.base);

    if (!edir)
        return elog(1, "could not open task directory");

    while ((ent = readdir(edir)) != NULL) {
        if (ent->d_name[0] == '.'  || ent->d_type != DT_DIR)
            continue;

        // TODO: simplify this logic
        if (cenv != NULL && strcmp(column_getcenv(), ent->d_name) == 0)
            mark = '*';
        else if (penv != NULL && strcmp(column_getpenv(), ent->d_name) == 0)
            mark = '^';
        else
            mark = '+';
        printf("%c %-10s [%s] %s\n", mark, ent->d_name, "roach", "some env desc");
    }
    closedir(edir);
    return 1;
}

int _env_prev(int argc, char **argv, tman_ctx_t *ctx)
{
    int status;
    struct tman_env_prev_opt opt;
    const char *errfmt = "cannot switch: %s";

    if ((status = tman_env_prev(ctx, &opt)) != TMAN_OK)
        return elog(status, errfmt, tman_strerror());
    return tman_pwd();
}

int _env_set(int argc, char **argv, tman_ctx_t *ctx)
{
    return 0;
}

int _env_use(int argc, char **argv, tman_ctx_t *ctx)
{
    int status;
    char *env;
    const char *errfmt = "cannot switch to '%s': %s";
    struct tman_env_use_opt opt;

    // TODO: throws error if switch to current env
    if ((env = argv[1]) == NULL)
        return elog(1, errfmt, "NOENV", "environment name required");
    else if ((status = tman_env_use(ctx, env, &opt)) != TMAN_OK)
        return elog(status, errfmt, env, tman_strerror());
    return tman_pwd();
}

int tman_cli_env(int argc, char **argv, tman_ctx_t *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(envcmds); ++i)
        if (strncmp(cmd, envcmds[i].name, CMDSIZ) == 0)
            return envcmds[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "no such env command '%s'", cmd);
}
