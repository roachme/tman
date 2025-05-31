#ifndef LIBTMAN_UNIT_H
#define LIBTMAN_UNIT_H

#define KEYSIZ 20
#define VALSIZ 80

struct unit {
    char *key;
    char *val;
    struct unit *next;
};

void unit_free(struct unit *units);
int unit_generate_prj(char *prj);
int unit_generate(char *prj, char *id);
struct unit *unit_load(char *filename);
char *unit_get(struct unit *head, char *key);
int unit_set(struct unit *head, char *key, char *val);
int unit_save(char *filename, struct unit *units);
struct unit *unit_add(struct unit *head, char *key, char *val);

#endif
