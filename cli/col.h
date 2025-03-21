#ifndef TMAN_COL_H
#define TMAN_COL_H

struct tman_mcol_opt {
    char *prj;                  /* project to add a task to */
    char *col;                  /* column tag to move a task to */
    int force;                  /* ignore existent task ID */
    int help;                   /* Show command usage */
};

#endif
