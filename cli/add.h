/*
    Add a task to a specified or current project.
    If multiple task IDs are passed then switch to last
    successfully added task ID.
*/

#ifndef LIBTMAN_ADD_H
#define LIBTMAN_ADD_H

enum tman_cli_add_errno {
    TMAN_ADD_IDEXT = 20,        /* task ID already exists */
    TMAN_ADD_IDREQ = 21,        /* task ID required */
};

#endif
