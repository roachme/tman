#include <libconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "aux/config.h"

// TODO: it ain't true config value cuz might be overwritten by CLI options
// TODO: Add support to specify config keys
static int _cfg_show(int argc, char **argv, tman_ctx_t * ctx)
{
    struct tman_hook *hook;

    printf("base.task: %s\n", tmancfg->base.task);
    printf("base.pgn: %s\n", tmancfg->base.pgn);
    puts("");
    printf("options.usehook: %s\n", tmancfg->usehooks ? "True" : "False");
    printf("options.usecolor: %s\n", tmancfg->usecolor ? "True" : "False");
    printf("options.usedebug: %s\n", tmancfg->usedebug ? "True" : "False");
    puts("");
    printf("hooks:show\n");
    for (hook = tmancfg->hooks; hook != NULL; hook = hook->next)
        if (strcmp(hook->pgntag, "HOOKSHOW") == 0)
            printf("  %-5s %s\n", hook->cmd, hook->pgname);
    puts("");
    printf("hooks:action\n");
    for (hook = tmancfg->hooks; hook != NULL; hook = hook->next)
        if (strcmp(hook->pgntag, "HOOKCMD") == 0)
            printf("  %-5s %s\n", hook->cmd, hook->pgname);
    return 0;
}

static const builtin_t cfgcmds[] = {
    {.name = "show",.func = &_cfg_show},
};

int tman_cli_cfg(int argc, char **argv, tman_ctx_t * ctx)
{
    int c;
    char *command;
    int showhelp, quiet;

    showhelp = quiet = FALSE;
    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }
    if (showhelp == TRUE)
        return help_usage("cfg");

    command = argc > optind ? argv[optind++] : "show";

    for (int i = 0; i < ARRAY_SIZE(cfgcmds); ++i)
        if (strncmp(command, cfgcmds[i].name, CMDSIZ) == 0)
            return cfgcmds[i].func(argc - optind, argv + optind, ctx);

    return elog(1, "'%s': no such cfg command", command);
}
