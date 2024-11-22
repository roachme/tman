#ifndef UNIT_H
#define UNIT_H

#define MAXUBIN     5   /* number of builtin unit values */
#define MAXUPGN     10  /* number of plugin unit values */
#define UKEYSIZ     10  /* max size of unit key */
#define UVALSIZ     80  /* max size of unit value */

struct pair {
    int isset;
    char key[UKEYSIZ + 1];
    char val[UVALSIZ + 1];
};

struct bunit {
    int size;
    struct pair pair[MAXUBIN];
};

struct punit {
    int size;
    struct pair pair[MAXUPGN];
};

/* Keep builtin and plugin units separately, this way it's
 * easier to save 'em into the separate unit files.  */
struct units {
    struct bunit bin;
    struct punit pgn;
};

int unit_add(char *env, char *id);
int unit_del(char *env, char *id);
struct bunit *unit_get(struct bunit *u, char *env, char *id);
int unit_set(char *env, char *id, struct bunit *bunit);
int unit_set2(char *env, char *id, char *key, char *val);

#endif
