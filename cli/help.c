#include <stdio.h>
#include <string.h>

#include "help.h"
#include "cli.h"

#define TAGSYSTEM   "bin-system"
#define TAGBASIC    "bin-basic"
#define TAGINFO     "bin-info"

struct help helptab[] = {
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
        .name  = "env",
        .synop = PROGRAM " env SUBCMD [OPTION] NAME",
        .sdesc = "manipulate environments",
        .desc  = "env description",
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
                 "  -n      don't switch to newly created task\n"
                 "  -q      do not write anything to standard error output"
    },
    {
        .tag = TAGBASIC,
        .name  = "col",
        .synop = "Usage: " PROGRAM " col [OPTION]... COLUMN [ID]...",
        .sdesc = "Move a task to new column",
        .desc  = "With no option `-e' operate on current environment.\n"
                 "COLUMN is column to move a task to.\n"
                 "Options:\n"
                 "  -e      specify environment name\n"
                 "  -l      list environment columns\n"
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
        .synop = "Usage: " PROGRAM " prev",
        .sdesc = "Switch to previous task",
    },
    {
        .tag = TAGBASIC,
        .name  = "move",
        .synop = PROGRAM " move [OPTION] ID DST SRC",
        .sdesc = "rename or move task",
        .desc  = "move description",
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
        .synop = "Usage: " PROGRAM " sync",
        .sdesc = "Synchronize task",
    },
    {
        .tag = TAGBASIC,
        .name  = "use",
        .synop = PROGRAM " use ID [ENV]",
        .sdesc = "switch to specific task",
        .desc  = "use description",
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
        .synop = "Usage: " PROGRAM " list [OPTION]... ENV",
        .sdesc = "List environment tasks",
        .desc  = "With no option `-e' ENV is current environment.\n"
                 "  -A      list all tasks\n"
                 "  -a      list almost all tasks (expect for archieved)\n"
                 "  -h      show this help and exit\n"
                 "  -t      list only current and previous tasks\n"
    },
    {
        .tag = TAGINFO,
        .name  = "ver",
        .synop = PROGRAM " ver",
        .sdesc = "show version and exit",
        .desc  = "ver description",
    },
};

int help_list_commands(void)
{
    printf("Usage: %s [OBJECT] COMMAND [OPITONS]... [ID]...\n", PROGRAM);
    printf("    Where OBJECT := { id | env | pgm }\n");
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

    printf("\nInfo:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGINFO) == 0)
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
            printf("%s\n", helptab[i].desc);
            return TMAN_OK;
        }
    }
    return emod_set(TMAN_CMD_BIN);
}

int tman_cli_help(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    char *cmd;
    //char *key = NULL;
    int status;

    while ((c = getopt(argc, argv, "dk:")) != -1) {
        switch (c) {
            case 'k':
                //key = optarg;
                break;
        };
    }

    cmd = argv[optind];
    if ((status = help_lookup(cmd)) != TMAN_OK)
        return elog(status, "cannot find '%s': %s", cmd, tman_strerror());
    return TMAN_OK;
}
