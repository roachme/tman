#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

int column_loadids(char *env);
int column_saveids(char *env);

int column_init(char *env);
int column_delpid(void);
int column_delcid(void);
int column_swapids(void);
int column_mark(char *env, char *id); /* mark newly added task with blog column */
int column_addcid(char *id);

#endif
