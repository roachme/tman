#ifndef LIBTMAN_LINK_H
#define LIBTMAN_LINK_H

int link_set_parent(char *prj, char *pid, char *cid);
int link_set_child(char *prj, char *pid, char *cid);
int link_get_parent(char *prj, char *id, char *linkname);
int link_get_child(char *prj, char *id, char *linkname);

#endif
