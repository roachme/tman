#include <stdio.h>
#include <string.h>
#include "common.h"
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
        .synop = PROGRAM " add ID... [ENV]",
        .sdesc = "add a new task",
        .desc  = "add description"
                 " and these too" ,
    },
    {
        .tag = TAGBASIC,
        .name  = "del",
        .synop = PROGRAM " del ID... [ENV]",
        .sdesc = "delete a task",
        .desc  = "del description",
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

int help_lookup(const char *cmd)
{
    if (cmd == NULL)
        return help_list_commands();
    for (int i = 0; i < helpsize; ++i) {
        if (strcmp(help[i].name, cmd) == 0) {
            printf("%s\n", help[i].sdesc);
            printf("%s\n", help[i].synop);
            return 1;
        }
    }
    return elog("cannot access '%s': commnd not found", cmd);
}
