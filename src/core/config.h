#ifndef CONFIG_H
#define CONFIG_H

extern struct config config;

struct hook {
    char hook[10];
    char cmd[10];
    char cmdopt[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
};

struct config {
    int hooknum;
    char base[100];
    char pgnins[100];
    struct hook hooks[10];
};

int parseconf(const char *fname);

#endif
