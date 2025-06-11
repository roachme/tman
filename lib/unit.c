#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "unit.h"
#include "common.h"

int unit_save(char *filename, struct unit *units)
{
    FILE *fp;

    if ((fp = fopen(filename, "w")) == NULL)
        return 1;

    for (; units; units = units->next)
        fprintf(fp, "%s : %s\n", units->key, units->val);
    return fclose(fp);
}

/*
 * Get units all units.
*/
struct unit *unit_load(char *filename)
{
    FILE *fp;
    struct unit *units;
    char buf[BUFSIZ + 1];
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];

    if ((fp = fopen(filename, "r")) == NULL)
        return NULL;

    // TODO: write a better parser
    units = NULL;
    while (fgets(buf, BUFSIZ, fp) != NULL) {
        char *token;
        if ((token = strtok(buf, " :\n")) != NULL)
            strcpy(key, token);
        if ((token = strtok(NULL, ":\n")) != NULL)
            strcpy(val, token);
        units = unit_add(units, key, &val[1]);
    }
    fclose(fp);
    return units;
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

int unit_generate_prj(char *prj)
{
    struct unit *units = NULL;
    char desc[100] = "autogenerate desciption for project ";

    strcat(desc, prj);
    units = unit_add(units, "desc", desc);
    if (unit_save(genpath_unit_prj(prj), units))
        return 1;
    // roach: make return value int ??
    unit_free(units);
    return 0;
}

int unit_generate(char *prj, char *id)
{
    char buff[BUFSIZ + 1];
    struct unit *units = NULL;
    time_t rawtime = time(NULL);
    const char timefmt[] = "%Y%m%d";
    struct tm *timeinfo = localtime(&rawtime);
    char desc[100] = "autogenerate desciption for ";

    strcat(desc, id);
    strftime(buff, BUFSIZ, timefmt, timeinfo);
    units = unit_add(units, "prio", "mid");
    units = unit_add(units, "type", "task");
    units = unit_add(units, "date", buff);
    units = unit_add(units, "desc", desc);
    if (unit_save(genpath_unit(prj, id), units)) {
        fprintf(stderr, "unit_save_bin\n");
        return 1;
    }
    // roach: make return value int ??
    unit_free(units);
    return 0;
}

/*
 * Set units one at a time.
*/
int unit_set(struct unit *head, char *key, char *val)
{
    for (; head != NULL; head = head->next) {
        if (strcmp(head->key, key) == 0) {
            free(head->val);
            head->val = strdup(val);
            return 0;
        }
    }
    return 1;
}

char *unit_get(struct unit *head, char *key)
{
    while (head) {
        if (strcmp(head->key, key) == 0)
            return head->val;
        head = head->next;
    }
    return NULL;
}

/*
 * Delete all units.
*/
void unit_free(struct unit *units)
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
