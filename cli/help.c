#include <stdio.h>
#include <string.h>

#include "../src/tman.h"
#include "help.h"

#define TAGSYSTEM   "bin-system"
#define TAGBASIC    "bin-basic"
#define TAGMISC     "bin-misc"
#define TAGINFO     "bin-info"

struct help help[] = {
    {
        .tag = TAGSYSTEM,
        .name  = "cfg",
        .sdesc = "manipulate system configuration",
        .desc  = "cfg description"
    },
    {
        .tag = TAGSYSTEM,
        .name  = "chk",
        .sdesc = "check system config and task units",
        .desc  = "cfg description"
    },
    {
        .tag = TAGSYSTEM,
        .name  = "init",
        .sdesc = "init directory structure",
        .desc  = "init description"
    },
    {
        .tag = TAGSYSTEM,
        .name  = "pgm",
        .sdesc = "plugin manager",
        .desc  = "pgm description"
    },

    {
        .tag = TAGBASIC,
        .name  = "add",
        .synop = "Usage: " PROGRAM " add [OPTION]... ID...",
        .sdesc = "Add a new task into environment",
        .desc  = "With no option `-e' add a new task to current environment.\n"
                 "  -e      specify an environment to add a task to\n"
                 "  -f      do not interrupt if task exists, process others\n"
                 "  -h      show this help and exit\n"
                 "  -n      don't switch to newly created task (default is false)\n"
                 "  -q      do not write anything to standard error output"
    },
    {
        .tag = TAGBASIC,
        .name  = "col",
        .synop = "Usage: " PROGRAM " col COLUMN [OPTION]... ID...",
        .sdesc = "Move a task to a column",
        .desc  = "With no option `-e' add a new task to current environment.\n"
                 "COLUMN is column to move a task to\n"
                 "  -h      show this help and exit"
    },
    {
        .tag = TAGBASIC,
        .name  = "del",
        .synop = "Usage: " PROGRAM " del [OPTION]... ID..\n"
                 "Try '" PROGRAM " help del' for more info.",
        .sdesc = "Delete the task from the environment",
        .desc  = "With no option `-e' delete task from current environment.\n"
                 "  -e      specify an environment to add a task to\n"
                 "  -f      do not interrupt if task does not exist, process others\n"
                 "  -h      show this help and exit\n"
                 "  -q      do not write anything to standard error output"
    },
    {
        .tag = TAGBASIC,
        .name  = "prev",
        .synop = PROGRAM " prev",
        .sdesc = "switch to previous task",
        .desc  = "prev description",
    },
    {
        .tag = TAGBASIC,
        .name  = "set",
        .synop = PROGRAM " set OPTION... [ID]... [ENV]",
        .sdesc = "set task unit values",
        .desc  = "set description",
    },
    {
        .tag = TAGBASIC,
        .name  = "sync",
        .synop = PROGRAM " sync",
        .sdesc = "synchronize task",
        .desc  = "sync description",
    },
    {
        .tag = TAGBASIC,
        .name  = "use",
        .synop = PROGRAM " use ID [ENV]",
        .sdesc = "switch to specific task",
        .desc  = "use description",
    },

    {
        .tag = TAGMISC,
        .name  = "env",
        .synop = PROGRAM " env SUBCMD [OPTION] NAME",
        .sdesc = "manipulate environments",
        .desc  = "env description",
    },
    {
        .tag = TAGMISC,
        .name  = "move",
        .synop = PROGRAM " move [OPTION] ID DST SRC",
        .sdesc = "move task to another environment",
        .desc  = "move description",
    },

    {
        .tag = TAGINFO,
        .name  = "cat",
        .synop = PROGRAM " cat ID... [ENV]",
        .sdesc = "cat task info",
        .desc  = "cat description",
    },
    {
        .tag = TAGINFO,
        .name  = "help",
        .synop = PROGRAM " help [CMD]",
        .sdesc = "show this help message and exit",
        .desc  = "help description",
    },
    {
        .tag = TAGINFO,
        .name  = "list",
        .synop = PROGRAM " list [ENV]",
        .sdesc = "list environment task",
        .desc  = "help description",
    },
    {
        .tag = TAGINFO,
        .name  = "ver",
        .synop = PROGRAM " ver",
        .sdesc = "show version and exit",
        .desc  = "ver description",
    },
};

static size_t helpsize = sizeof(help) / sizeof(help[0]);

int help_list_commands(void)
{
    printf("Usage: %s [OBJECT] COMMAND [OPITONS]... [ID]...\n", PROGRAM);
    printf("    Where OBJECT := { id | env | pgm }\n");
    printf("    Where COMMAND is one of the builtin commands\n");
    printf("    Where ID is task ID\n");
    printf("Try '" PROGRAM " help COMMAND' for more info.\n\n");

    printf("System:\n");
    for (int i = 0; i < helpsize; ++i)
        if (strcmp(help[i].tag, TAGSYSTEM) == 0)
            printf("  %-6s - %s\n", help[i].name, help[i].sdesc);

    printf("\nBasic:\n");
    for (int i = 0; i < helpsize; ++i)
        if (strcmp(help[i].tag, TAGBASIC) == 0)
            printf("  %-6s - %s\n", help[i].name, help[i].sdesc);

    printf("\nMisc:\n");
    for (int i = 0; i < helpsize; ++i)
        if (strcmp(help[i].tag, TAGMISC) == 0)
            printf("  %-6s - %s\n", help[i].name, help[i].sdesc);

    printf("\nInfo:\n");
    for (int i = 0; i < helpsize; ++i)
        if (strcmp(help[i].tag, TAGINFO) == 0)
            printf("  %-6s - %s\n", help[i].name, help[i].sdesc);

    return 1;
}

int help_usage(const char *cmd)
{
    for (int i = 0; i < helpsize; ++i) {
        if (strcmp(help[i].name, cmd) == 0) {
            printf("%s\n", help[i].synop);
            printf("%s\n", help[i].sdesc);
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
    for (int i = 0; i < helpsize; ++i) {
        if (strcmp(help[i].name, cmd) == 0) {
            printf("%s\n", help[i].synop);
            printf("%s\n\n", help[i].sdesc);
            printf("%s\n", help[i].desc);
            return 1;
        }
    }
    return elog(1, "cannot access '%s': command not found", cmd);
}

int tman_cli_help(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    char *cmd;
    char *key = NULL;

    while ((c = getopt(argc, argv, "dk:")) != -1) {
        switch (c) {
            case 'k':
                key = optarg; break;
        };
    }

    cmd = argv[optind];
    return help_lookup(cmd);
}
