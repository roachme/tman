#ifndef UNIT_H
#define UNIT_H

#define BINSIZ      6
#define PGNSIZ      10
#define HOOKSIZ     1000

#define USIZE 80

struct pair {
    int isset;
    char key[USIZE];
    char val[USIZE];
};

struct bunit {
    int size;
    struct pair pair[BINSIZ];
};

struct punit {
    int size;
    struct pair pair[PGNSIZ];
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
int unit_set(char *env, char *id, struct bunit *bunit);

#endif
