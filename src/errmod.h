#ifndef TMAN_ERRMOD_H
#define TMAN_ERRMOD_H

#define ERRMSGSIZ   100
#define NERRCODE    50

enum tman_err_enum {
    TMAN_OK,        /* OK, interpret output as path or output */
    TMAN_ECORE,    /* ?? Error in core execution (under consideration to include) */
    TMAN_ESYSVAR,
    TMAN_EINIT,
    TMAN_EMKDIR,
    TMAN_ECONF,
    TMAN_EHOOK,    /* Failed to execute hook */
    TMAN_EPGN,    /* Failed to execute plugin */
    TMAN_EOPENENVDIR,
    TMAN_EINIT_ENVMOD,

    TMAN_ENV_SWAP,
    TMAN_ENV_ILLEG,
    TMAN_ENV_NOCURR,
    TMAN_ENV_NOPREV,
    TMAN_ENV_NOSUCH,
    TMAN_ENV_EXISTS,
    TMAN_ENV_SWITCH,

    TMAN_ID_ILLEG,
    TMAN_ID_NOCURR,
    TMAN_ID_NOPREV,
    TMAN_ID_NOSUCH,
    TMAN_ID_EXISTS,
    TMAN_ID_SWAP,

    TMAN_COL_ADD,
    TMAN_COL_MOVE,
    TMAN_COL_DEL,
    TMAN_COL_EXISTS,

    TMAN_DIR_ID_MAKE,
    TMAN_DIR_ID_DEL,
    TMAN_UNIT_MAKE,
    TMAN_UNIT_DEL,
    TMAN_DIR_ID_OPEN,

    TMAN_DIR_ENV_DEL,
    TMAN_DIR_ENV_MAKE,
    TMAN_DIR_ENV_OPEN,

    TMAN_UNIT_SET,
    TMAN_UNIT_GET,
    TMAN_UNIT_ILLEG,

    TMAN_CMD_BIN,
    TMAN_CMD_PGN,
    TMAN_CMD_GEN,

    TMAN_NODEF_ERR,

    __TMAN_STATUS_LAST
};

struct tman_err_codes {
    enum tman_err_enum err;
    char desc[ERRMSGSIZ + 1];
};

int emod_set(int err);
const char *emod_strerror(void);

#endif
