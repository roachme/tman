#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unit.h"
#include "common.h"

int unit_save(char *prj, char *id, struct unit *units)
{
    FILE *fp;

    if ((fp = fopen(genpath_unit(prj, id), "w")) == NULL)
        return 1;

    for (; units; units = units->next)
        fprintf(fp, "%s : %s\n", units->key, units->val);
    return fclose(fp);
}

/*
 * Get units one at a time.
*/
struct unit *unit_load(char *prj, char *id)
{
    FILE *fp;
    struct unit *units;
    char buf[BUFSIZ + 1];
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char *token;

    if ((fp = fopen(genpath_unit(prj, id), "r")) == NULL)
        return NULL;

    // TODO: write a better parser
    units = NULL;
    while (fgets(buf, BUFSIZ, fp) != NULL) {
        if ((token = strtok(buf, " :\n")) != NULL)
            strcpy(key, token);
        if ((token = strtok(NULL, ":\n")) != NULL)
            strcpy(val, token);
        units = unit_add(units, key, &val[1]);
    }
    fclose(fp);
    return units;
}

void unit_traverse(struct unit *head)
{
    for (; head; head = head->next)
        printf("traverse: %s : '%s'\n", head->key, head->val);
}

/*
 * Add units one at a time.
*/
struct unit *unit_add(struct unit *head, char *key, char *val)
{
    struct unit *unit, *tmp;

    if ((unit = malloc(sizeof(struct unit))) == NULL)
        return head;

    unit->next = NULL;
    unit->key = strdup(key);
    unit->val = strdup(val);

    if ((tmp = head) == NULL)
        return unit;

    for (tmp = head; tmp && tmp->next; tmp = tmp->next) ;
    tmp->next = unit;
    return head;
}

/*
 * Set units one at a time.
*/
int unit_set(struct unit *head, char *key, char *val)
{
    for (; head != NULL; head = head->next) {
        if (strcmp(head->key, key) == 0) {
            strcpy(head->val, val);
            return 0;
        }
    }
    return 1;
}

/*
 * Delete all units.
*/
void unit_del(struct unit *units)
{
    struct unit *tmp;

    while (units) {
        tmp = units->next;
        free(units->key);
        free(units->val);
        free(units);
        units = tmp;
    }
}
