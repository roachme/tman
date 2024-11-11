#ifndef HELP_H
#define HELP_H

#define HELPSIZE 10

struct help {
    char tag[100];
    char name[100];
    char synop[100];
    char sdesc[1000];
    char desc[1000];
};



int help_lookup(const char *cmd);

int help_bincmd(const char *cmd);
int help_envcmd(const char *cmd);
int help_pgmcmd(const char *cmd);

#endif
