#include <stdio.h>
#include <string.h>

#include "help.h"
#include "cli.h"

#define TAGOBJ      "bin-object"
#define TAGOBJCMD   "bin-object-cmd"
#define TAGSYSTEM   "bin-system"
#define TAGBASIC    "bin-basic"
#define TAGMISC     "bin-misc"
#define TAGINFO     "bin-info"

struct helpctx {
    int synop;
    int desc_short;
    int desc_long;
};

struct helpctx helpctx = {
    .synop = FALSE,
    .desc_short = FALSE,
    .desc_long = FALSE,
};

struct help helptab[] = {
    {
     .tag = TAGSYSTEM,
     .name = "cfg",
     .synop = "Usage: " PROGRAM " cfg\n",
     .desc_short = "Manage system configuration file.\n",
     .desc_long = "\n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGSYSTEM,
     .name = "chk",
     .synop = "Usage: " PROGRAM " chk\n",
     .desc_short = "Check and repair a system.\n",
     .desc_long = "\n\
    Options:\n\
      -s      under development: soft fix (without losing any user data)\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGSYSTEM,
     .name = "help",
     .synop = "Usage: " PROGRAM " help [OPTION] [CMD]\n",
     .desc_short = "Show help message for command.\n",
     .desc_long = "\n\
    If no CMD passed, list all commands with short description.\n\
    \n\
    Options:\n\
      -d      output short description for each topic\n\
      -s      output only a short usage synopsis for each topic matching\n\
    \n\
    Arguments:\n\
      CMD     builtin command\n\
    \n\
    Exit status:\n\
    Returns success unless PATTERN is not found or an invalid option is given.\n"},
    {
     .tag = TAGSYSTEM,
     .name = "init",
     .synop = "Usage: " PROGRAM " init\n",
     .desc_short = "Init directory structure.\n",
     .desc_long = "\n\
    Exit status:\n\
    Under development\n"},

    {
     .tag = TAGBASIC,
     .name = "add",
     .synop = "Usage: " PROGRAM " add [OPTION]... ID...\n",
     .desc_short = "Add a new task to project.\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -n      don't switch to newly created task\n\
      -p PRJ  project name (default is current)\n\
      -q      do not write anything to standard error output\n\
    \n\
    Arguments:\n\
      ID      task ID\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGBASIC,
     .name = "del",
     .synop = "Usage: " PROGRAM " del [OPTION]... [ID]...\n",
     .desc_short = "Delete task from project.\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -n      delete task without confirmation\n\
      -p PRJ  project name (default is current)\n\
      -q      do not write anything to standard error output\n\
    \n\
    Arguments:\n\
      ID      task ID (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGMISC,
     .name = "find",
     .synop = "Usage: " PROGRAM " find [OPTION]... PATTERN\n",
     .desc_short = "Find a task by pattern (under development).\n",
     .desc_long = "\n\
    NOTE:\n\
        UNDER DEVELOPMENT\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -p PRJ  project name (default is current)\n\
    \n\
    Arguments:\n\
      PATTERN      Pattern specifying a description topic\n\
      PRJ          project name (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGMISC,
     .name = "flow",
     .synop = "Usage: " PROGRAM " flow [OPTION]... FLOW\n",
     .desc_short = "Show and set project workflow (under development).\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -p PRJ  project name (default is current)\n\
    \n\
    Arguments:\n\
      PATTERN      Pattern specifying a description topic\n\
      PRJ          project name (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGBASIC,
     .name = "list",
     .synop = "Usage: " PROGRAM " list [OPTION]... [PRJ]...\n",
     .desc_short = "List project tasks.\n",
     .desc_long = "\n\
    Options:\n\
      -A      list all tasks\n\
      -a      list almost all tasks (expect for done)\n\
      -c COL  specify what column to list task from (under development)\n\
      -h      show this help and exit\n\
      -s      list only current and previous tasks\n\
    \n\
    Arguments:\n\
      PRJ     project name (default current)\n\
      COL     project column name\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGBASIC,
     .name = "prev",
     .synop = "Usage: " PROGRAM " prev [OPTION]...\n",
     .desc_short = "Switch to previous task in current project.\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Return success if previous task and current project exists, otherwise fail\n"},
    {
     .tag = TAGBASIC,
     .name = "show",
     .synop = "Usage: " PROGRAM " show [OPTION]... [ID]...\n",
     .desc_short = "Show task info.\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -k key  key to show (builtin or plugin)\n\
      -p PRJ  project name (default is current)\n\
    \n\
    Arguments:\n\
      ID      task ID (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGBASIC,
     .name = "sync",
     .synop = "Usage: " PROGRAM " sync [OPTION]... [ID]...\n",
     .desc_short = "Switch to or synchronize (with) task(s).\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -n      do not switch to task\n\
      -p PRJ  project name (default is current)\n\
      -q      do not write anything to standard error output\n\
    \n\
    Arguments:\n\
      ID      task ID (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},

    {
     .tag = TAGOBJ,
     .name = "prj",
     .synop = "Usage: " PROGRAM " prj SUBCMD [OPTION] NAME\n",
     .desc_short = "Manage and show projects.\n",
     .desc_long = "\n\
    Arguments:\n\
      SUBCMD  project subcommand\n\
    \n\
    SUBCMD list:\n\
      add     Add a new project\n\
      del     Delete project with all tasks\n\
      list    List projects\n\
      prev    Switch to previous project\n\
      rename  Rename project\n\
      set     Set project vaules (under development)\n\
      show    Show project info (under development)\n\
      sync    Switch to or synchronize (with) project\n\
    \n\
    Note:\n\
      Use '" PROGRAM " help prj-SUBCMD' to get help on subcommands. \n\
      For example: " PROGRAM " help prj-add\n\
    \n\
    Exit status:\n\
    The return status is return status of subcommand.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-add",
     .synop = "Usage: " PROGRAM " prj add [OPTION] NAME...\n",
     .desc_short = "Add new project(s).\n",
     .desc_long = "\n\
    Arguments:\n\
      NAME    project name\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -n      do not switch to newly created project\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-del",
     .synop = "Usage: " PROGRAM " prj del [OPTION] NAME...\n",
     .desc_short = "Delete project(s).\n",
     .desc_long = "\n\
    Arguments:\n\
      NAME    project name (default current)\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-list",
     .synop = "Usage: " PROGRAM " prj list [OPTION]\n",
     .desc_short = "List project(s).\n",
     .desc_long = "\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-prev",
     .synop = "Usage: " PROGRAM " prj prev\n",
     .desc_short = "Switch to previous project.\n",
     .desc_long = "\n\
    Arguments:\n\
      NAME    project name\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-rename",
     .synop = "Usage: " PROGRAM " prj rename [OPTION]... SRC DST\n",
     .desc_short = "Rename project.\n",
     .desc_long = "\n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-set",
     .synop = "Usage: " PROGRAM " prj set [OPTION] [NAME]...\n",
     .desc_short = "List project(s).\n",
     .desc_long = "\n\
    This feature is under development.\n\
    Arguments:\n\
      NAME    project name\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-show",
     .synop = "Usage: " PROGRAM " prj show [OPTION] [NAME]...\n",
     .desc_short = "Show project(s) info.\n",
     .desc_long = "\n\
    This feature is under development.\n\
    Arguments:\n\
      NAME    project name\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},
    {
     .tag = TAGOBJCMD,
     .name = "prj-sync",
     .synop = "Usage: " PROGRAM " prj sync [OPTION] NAME...\n",
     .desc_short = "Switch to or synchronize (with) project(s).\n",
     .desc_long = "\n\
    Arguments:\n\
      NAME    project name\n\
    \n\
    Options:\n\
      -h      show this help and exit\n\
      -n      do not switch to project\n\
      -q      do not write anything to standard error output\n\
    \n\
    Exit status:\n\
    Under development.\n"},

    {
     .tag = TAGOBJ,
     .name = "pgm",
     .synop = "Usage: " PROGRAM " pgm PLUGIN [OPTION]...\n",
     .desc_short = "Manage and show plugins.\n",
     .desc_long = "\n\
    Arguments:\n\
      PLUGIN  plugin name\n\
    \n\
    Exit status:\n\
    The return status is return status of plugin.\n"},

    {
     .tag = TAGMISC,
     .name = "col",
     .synop = "Usage: " PROGRAM " col [OPTION]... COLUMN [ID]...\n",
     .desc_short = "Move task to column.\n",
     .desc_long = "\n\
    Options:\n\
      -l      list project columns\n\
      -h      show this help and exit\n\
      -p PRJ  project name (default is current)\n\
    \n\
    Arguments:\n\
      ID      task ID (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},

    {
     .tag = TAGMISC,
     .name = "link",
     .synop = "Usage: " PROGRAM " link [OPTION]... PID CID\n",
     .desc_short = "Link tasks together (under development).\n",
     .desc_long = "\n\
    Options:\n\
      -c      link task as child\n\
      -l      link two tasks\n\
      -p PRJ  project name (default is current)\n\
    \n\
    Arguments:\n\
      CID     child task ID\n\
      PID     parent task ID\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGMISC,
     .name = "move",
     .synop = "Usage: " PROGRAM " move [OPTION]... [SRC DST | SRC...]\n",
     .desc_short = "Move (rename) tasks.\n",
     .desc_long = "\n\
    Options:\n\
      -f      overwrite destination task (under development)\n\
      -d      destination project\n\
      -s      source project\n\
    \n\
    Exit status:\n\
    Under development\n"},
    {
     .tag = TAGMISC,
     .name = "set",
     .synop = "Usage: " PROGRAM " set OPTION... [ID]...\n",
     .desc_short = "Set task unit values.\n",
     .desc_long = "\n\
    Options:\n\
      -d      task description\n\
      -p PRJ  project name (default is current)\n\
      -P      task priority\n\
      -t      task type\n\
    \n\
    Arguments:\n\
      ID      task ID (default current)\n\
    \n\
    Exit status:\n\
    Under development\n"},
};

int help_list_commands(void)
{
    printf("Usage: %s [OPTION]... COMMAND [OPITONS]... [ARGS]...\n\n", PROGRAM);
    printf("These are common Tman commands used in various situations:\n\n");

    printf("System:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGSYSTEM) == 0)
            printf("  %-6s - %s", helptab[i].name, helptab[i].desc_short);

    printf("\nBasic:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGBASIC) == 0)
            printf("  %-6s - %s", helptab[i].name, helptab[i].desc_short);

    printf("\nMisc:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGMISC) == 0)
            printf("  %-6s - %s", helptab[i].name, helptab[i].desc_short);

    printf("\nObject:\n");
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i)
        if (strcmp(helptab[i].tag, TAGOBJ) == 0)
            printf("  %-6s - %s", helptab[i].name, helptab[i].desc_short);

    return TMAN_OK;
}

int help_usage(const char *cmd)
{
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i) {
        if (strcmp(helptab[i].name, cmd) == 0) {
            printf("%s", helptab[i].synop);
            printf("%s", helptab[i].desc_short);
            printf("Try '" PROGRAM " help %s' for more info.\n", cmd);
            return 1;
        }
    }
    return elog(1, "%s: command not found", cmd);
}

int help_usage_description(const char *cmd)
{
    for (int i = 0; i < ARRAY_SIZE(helptab); ++i) {
        if (strcmp(helptab[i].name, cmd) == 0) {
            printf("%s\n", helptab[i].desc_short);
            return 1;
        }
    }
    return elog(1, "%s: command not found", cmd);
}

int help_lookup(const char *cmd)
{
    int i, found;

    if (cmd == NULL)
        return help_list_commands();

    found = 0;
    for (i = 0; i < ARRAY_SIZE(helptab); ++i) {
        if (strcmp(helptab[i].name, cmd) == 0) {
            found = 1;
            if (helpctx.synop) {
                printf("%s", helptab[i].synop);
                continue;
            }
            if (helpctx.desc_short) {
                printf("%s - %s", cmd, helptab[i].desc_short);
                continue;
            }

            printf("%s", helptab[i].synop);
            printf("    %s", helptab[i].desc_short);
            printf("    %s", helptab[i].desc_long);
            break;
        }
    }
    if (!found)
        return emod_set(TMAN_CMD_BIN);
    return TMAN_OK;
}

int tman_cli_help(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, status;

    while ((c = getopt(argc, argv, ":ds")) != -1) {
        switch (c) {
        case 'd':
            helpctx.desc_short = TRUE;
            helpctx.synop = FALSE;
            helpctx.desc_long = FALSE;
            break;
        case 's':
            helpctx.synop = TRUE;
            helpctx.desc_short = FALSE;
            helpctx.desc_long = FALSE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        };
    }

    if (optind == argc)
        return help_list_commands();

    for (i = optind; i < argc; ++i) {
        if ((status = help_lookup(argv[i])) != TMAN_OK)
            elog(status, "%s: %s", argv[i], tman_strerror());
    }
    return status;
}
