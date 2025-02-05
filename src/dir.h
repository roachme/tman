#ifndef DIR_H
#define DIR_H

int dir_id_add(char *base, char *prj, char *id);
int dir_id_del(char *base, char *prj, char *id);
int dir_id_ren(char *base, char *prj, char *newid, char *oldid);
int dir_id_move(char *base, char *id, char *dstprj, char *srcprj);

int dir_prj_add(char *base, char *prj);
int dir_prj_del(char *base, char *prj);

#endif
