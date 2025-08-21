#ifndef LIBTEC_UNIT_H
#define LIBTEC_UNIT_H

#include "libtec.h"

tec_unit_t *unit_load(const char *filename);
tec_unit_t *unit_parse(tec_unit_t * head, const char *str);
tec_unit_t *unit_add(tec_unit_t * head, const char *key, const char *val);
tec_unit_t *unit_set(tec_unit_t * head, const char *key, const char *val);
int unit_save(const char *filename, tec_unit_t * head);
char *unit_get(tec_unit_t * head, const char *key);
void unit_free(tec_unit_t * head);

#endif
