#include "common.h"

int link_set_parent(char *prj, char *pid, char *cid)
{
    FILE *fp;

    if ((fp = fopen(genpath_link(prj, pid), "w")) == NULL)
        return 1;
    fprintf(fp, "parent: %s\n", cid);
    return fclose(fp);
}

int link_set_child(char *prj, char *pid, char *cid)
{
    FILE *fp;

    if ((fp = fopen(genpath_link(prj, cid), "w")) == NULL)
        return 1;
    fprintf(fp, "child: %s\n", pid);
    return fclose(fp);
}

int link_get_parent(char *prj, char *id, char *linkname)
{
    FILE *fp;

    if ((fp = fopen(genpath_link(prj, id), "r")) == NULL)
        return 1;
    fscanf(fp, "parent: %s\n", linkname);
    return fclose(fp);
}

int link_get_child(char *prj, char *id, char *linkname)
{
    FILE *fp;

    if ((fp = fopen(genpath_link(prj, id), "r")) == NULL)
        return 1;
    fscanf(fp, "child: %s\n", linkname);
    return fclose(fp);
}
