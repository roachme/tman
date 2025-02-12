#include <stdio.h>
#include <string.h>

#include "help.h"
#include "cli.h"

#define TAGOBJ      "bin-object"
#define TAGSYSTEM   "bin-system"
#define TAGBASIC    "bin-basic"
#define TAGMISC     "bin-misc"
#define TAGINFO     "bin-info"

struct help helptab[] = {
    {
        .tag = TAGSYSTEM,
        .name  = "cfg",
        .synop = "Usage: " PROGRAM " cfg",
        .sdesc = "  Configuration manager",
    },
    {
        .tag = TAGSYSTEM,
        .name  = "chk",
        .synop = "Usage: " PROGRAM " chk",
        .sdesc = "  Check system health",
    },
    {
        .tag = TAGSYSTEM,
        .name  = "help",
        .synop = "Usage: " PROGRAM " help [CMD]",
        .sdesc = "  Show help message for CMD. Default list all commands.",
    },
    {
        .tag = TAGSYSTEM,
        .name  = "init",
        .synop = "Usage: " PROGRAM " init",
        .sdesc = "  Init directory structure",
    },
    {
        .tag = TAGSYSTEM,
        .name  = "ver",
        .synop = "Usage: " PROGRAM " ver",
        .sdesc = "  Show version and exit",
    },

    {
        .tag = TAGBASIC,
        .name  = "add",
        .synop = "Usage: " PROGRAM " add [OPTION]... ID...",
        .sdesc = "  Add a new task to project",
        .opts  = "  Options:\n"
            "    -f      do not interrupt if task exists, process others\n"
            "    -g      generate task ID (under development)\n"
            "    -h      show this help and exit\n"
            "    -n      don't switch to newly created task\n"
            "    -p      project name (default is current)\n"
            "    -q      do not write anything to standard error output"
    },
    {
        .tag = TAGBASIC,
        .name  = "del",
        .synop = "Usage: " PROGRAM " del [OPTION]... ID..",
        .sdesc = "  Delete task from project",
        .opts  = "  Options:\n"
            "    -f      do not interrupt if task does not exist, process others\n"
            "    -h      show this help and exit\n"
            "    -p      project to delete a task from (default current)\n"
            "    -q      do not write anything to standard error output"
    },
    {
        .tag = TAGMISC,
        .name  = "find",
        .synop = "Usage: " PROGRAM " find [OPTION]... PRJ",
        .sdesc = "find a task by ID/tag",
    },
    {
        .tag = TAGMISC,
        .name  = "flow",
        .synop = "Usage: " PROGRAM " flow [OPTION]... PRJ",
        .sdesc = "show and set project workflow",
    },
    {
        .tag = TAGMISC,
        .name  = "grep",
        .synop = "Usage: " PROGRAM " grep [OPTION]... PRJ",
        .sdesc = "find pattern in a task",
    },
    {
        .tag = TAGBASIC,
        .name  = "list",
        .synop = "Usage: " PROGRAM " list [OPTION]... PRJ",
        .sdesc = "  List project tasks",
        .opts  = "  Options:\n"
            "    -A      list all tasks\n"
            "    -a      list almost all tasks (expect for archieved)\n"
            "    -h      show this help and exit\n"
            "    -t      list only current and previous tasks\n"
    },
    {
        .tag = TAGBASIC,
        .name  = "prev",
        .synop = "Usage: " PROGRAM " prev",
        .sdesc = "  Switch to previous task",
    },
    {
        .tag = TAGBASIC,
        .name  = "show",
        .synop = PROGRAM " show ID... [PRJ]",
        .sdesc = "  Show task info",
        .opts  = "  Options:\n"
            "    -p      project name (default current)\n"
    },
    {
        .tag = TAGBASIC,
        .name  = "sync",
        .synop = "Usage: " PROGRAM " sync [OPTION]... [ID]...",
        .sdesc = "synchronize task",
        .opts  = "  Options:\n"
            "    -f      do not interrupt if task not found, process others\n"
            "    -h      show this help and exit\n"
            "    -n      don't switch to synced task\n"
            "    -p      project name (default current)\n"
            "    -q      do not write anything to standard error output"
    },
    {
        .tag = TAGBASIC,
        .name  = "use",
        .synop = "Usage: " PROGRAM " use [OPTION] ID",
        .sdesc = "  Switch to task by its ID",
        .opts  = "  Options:\n"
            "    -h      show this help and exit\n"
            "    -p      project name (default is current)\n"
    },

    {
        .tag = TAGOBJ,
        .name  = "prj",
        .synop = PROGRAM " prj SUBCMD [OPTION] NAME",
        .sdesc = "  Project manager",
    },
    {
        .tag = TAGOBJ,
        .name  = "pgm",
        .synop = "Usage: " PROGRAM " pgm PLUGIN [OPTION]...\n",
        .sdesc = "  Plugin manager",
    },

    {
        .tag = TAGMISC,
        .name  = "col",
        .synop = "Usage: " PROGRAM " col [OPTION]... COLUMN [ID]...",
        .sdesc = "move task to column",
        .opts  = "Options:\n"
            "    -p      project name (default is current)\n"
            "    -l      list project columns\n"
            "    -h      show this help and exit"
    },
    {
        .tag = TAGMISC,
        .name  = "link",
        .synop = "Usage: " PROGRAM " link [OPTION]... PID CID",
        .sdesc = "  Link tasks together",
        .opts  = "Options:\n"
            "    -c      link task as child\n"
            "    -l      link two tasks\n"
            "    -p      link task as parent\n"
    },
    {
        .tag = TAGMISC,
        .name  = "move",
        .synop = "Usage: " PROGRAM " move [OPTION]... [SRC DST | SRC...]",
        .sdesc = "  Move (rename) tasks",
        .opts  = "  Options:\n"
            "    -f      overwrite destination task\n"
            "    -d      destination project\n"
            "    -s      source project",

    },
    {
        .tag = TAGMISC,
        .name  = "set",
        .synop = "Usage: " PROGRAM " set OPTION... [ID]...",
        .sdesc = "  Set task unit values",
        .opts  = "Options:\n"
            "    -d      task description\n"
            "    -p      project name (default current)\n"
            "    -P      task priority\n"
            "    -t      task type\n"
    },
};

int help_list_commands(void)
{
    printf("Usage: %s [OBJECT] COMMAND [OPITONS]... [ID]...\n", PROGRAM);
    printf("    Where OBJECT := { id | prj | pgm }\n");
    printf("    Where COMMAND is one of the builtin commands\n");
    printf("    Where ID is task ID\n");
    printf("Try '" PROGRAM " help COMMAND' for more info.\n\n");

    printf("System:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGSYSTEM) == 0)
            printf("  %-6s - %s\n", helptab[i].name, helptab[i].sdesc);

    printf("\nBasic:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGBASIC) == 0)
            printf("  %-6s - %s\n", helptab[i].name, helptab[i].sdesc);

    printf("\nMisc:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGMISC) == 0)
            printf("  %-6s - %s\n", helptab[i].name, helptab[i].sdesc);

    printf("\nObject:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGOBJ) == 0)
            printf("  %-6s - %s\n", helptab[i].name, helptab[i].sdesc);

    return TMAN_OK;
}

int help_usage(const char *cmd)
{
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i) {
        if (strcmp(helptab[i].name, cmd) == 0) {
            printf("%s\n", helptab[i].synop);
            printf("%s\n", helptab[i].sdesc);
            printf("Try '" PROGRAM " help %s' for more info.\n", cmd);
            return 1;
        }
    }
    return elog(1, "cannot access '%s': command not found", cmd);
}

int help_lookup(const char *cmd)
{
    if (cmd == NULL)
        return help_list_commands();
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i) {
        if (strcmp(helptab[i].name, cmd) == 0) {
            printf("%s\n", helptab[i].synop);
            printf("%s\n\n", helptab[i].sdesc);
            //printf("%s\n\n", helptab[i].desc);
            printf("%s\n", helptab[i].opts);
            return TMAN_OK;
        }
    }
    return emod_set(TMAN_CMD_BIN);
}

int tman_cli_help(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    //char *key = NULL;
    int i, status;

    while ((c = getopt(argc, argv, "dk:")) != -1) {
        switch (c) {
            case 'k':
                //key = optarg;
                break;
        };
    }

    for (i = optind; i < argc; ++i) {
        if ((status = help_lookup(argv[i])) != TMAN_OK)
            return elog(status, "cannot find '%s': %s", argv[i], tman_strerror());
    }
    return status;
}
