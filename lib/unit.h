#ifndef LIBTMAN_UNIT_H
#define LIBTMAN_UNIT_H

#define KEYSIZ 20
#define VALSIZ 80

struct unit {
    int isset;
    char *key;
    char *val;
    struct unit *next;
};

enum {
    TMAN_UNIT_IDX_ID,
    TMAN_UNIT_IDX_PRIO,
    TMAN_UNIT_IDX_TYPE,
    TMAN_UNIT_IDX_DATE,
    TMAN_UNIT_IDX_DESC,
};

void unit_del(struct unit *units);
void unit_traverse(struct unit *head);
struct unit *unit_load(char *prj, char *id);
int unit_set(struct unit *head, char *key, char *val);
int unit_save(char *prj, char *id, struct unit *units);
struct unit *unit_add(struct unit *head, char *key, char *val);

#endif
