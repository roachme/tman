#ifndef HELP_H
#define HELP_H

struct help {
    const char *tag;
    const char *name;
    const char *synop;
    const char *desc_short;
    const char *desc_long;
};

int help_usage(const char *cmd);
int help_lookup(const char *cmd);

int help_bincmd(const char *cmd);
int help_prjcmd(const char *cmd);

#endif
