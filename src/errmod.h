#ifndef TMAN_ERRMOD_H
#define TMAN_ERRMOD_H

#define ERRSIZ      100
#define NERRCODE    50

struct errcodes {
    int err;
    char desc[ERRSIZ + 1];
};

#endif
