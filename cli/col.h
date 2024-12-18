#ifndef TMAN_COL_H
#define TMAN_COL_H

struct tman_mcol_opt {
    char *env;      /* environment to add a task to */
    char *col;      /* column tag to move a task to */
    int force;      /* ignore existent task ID */
    int help;       /* Show command usage */
};

int tman_cli_col(int argc, char **argv);

#endif
