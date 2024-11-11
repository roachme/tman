#ifndef UNIT_H
#define UNIT_H

#define UNITSIZ 5
#define HOOKSIZ 10

#define BUILTINSIZ  5
#define PLUGINSIZ   10

#define USIZE 80

struct pair {
    int isset;
    char key[USIZE];
    char val[USIZE];
};

struct unit {
    int size;
    struct pair pair[UNITSIZ];
};

struct bunit {
    int size;
    struct pair pair[BUILTINSIZ];
};

struct punit {
    int size;
    struct pair pair[PLUGINSIZ];
};

struct units {
    struct bunit builtn;
    struct punit plugin;
};


/*
struct unit {
    char id[20];
    char prio[20];
    char type[20];
    char date[100];
    char desc[100];
};
*/

int _chkid(char *id);
int _chkenv(char *env);

int unit_add(char *env, char *id);
int unit_del(char *env, char *id);
struct bunit *unit_get(struct bunit *u, char *env, char *id);
int unit_set(char *env, char *id, struct unit *unit);

#endif
