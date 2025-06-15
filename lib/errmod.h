#ifndef LIBTMAN_ERRMOD_H
#define LIBTMAN_ERRMOD_H

#define ERRMSGSIZ   100
#define NERRCODE    50

enum tman_err_enum {
    LIBTMAN_OK,                 /* OK, interpret output as path or output */
    LIBTMAN_ECORE,              /* ?? Error in core execution (under consideration to include) */
    LIBTMAN_ESYSVAR,
    LIBTMAN_EINIT,
    LIBTMAN_EFINIT,
    LIBTMAN_EMKDIR,
    LIBTMAN_ECONF,
    LIBTMAN_EHOOK,              /* Failed to execute hook */
    LIBTMAN_EPGN,               /* Failed to execute plugin */
    LIBTMAN_EOPENPRJDIR,
    LIBTMAN_EINIT_PRJMOD,
    LIBTMAN_SYS_FS,

    LIBTMAN_PRJ_LOAD,
    LIBTMAN_PRJ_SAVE,
    LIBTMAN_PRJ_SWAP,
    LIBTMAN_PRJ_TOOLONG,
    LIBTMAN_PRJ_ILLEG,
    LIBTMAN_PRJ_NOCURR,
    LIBTMAN_PRJ_NOPREV,
    LIBTMAN_PRJ_NOSUCH,
    LIBTMAN_PRJ_EXISTS,
    LIBTMAN_PRJ_SWITCH,
    LIBTMAN_PRJ_DEL_CURR,
    LIBTMAN_PRJ_DEL_PREV,
    LIBTMAN_PRJ_MISSING,

    LIBTMAN_BRD_NOCURR,

    LIBTMAN_ID_ILLEG,
    LIBTMAN_ID_TOOLONG,
    LIBTMAN_ID_NOCURR,
    LIBTMAN_ID_NOPREV,
    LIBTMAN_ID_NOSUCH,
    LIBTMAN_ID_EXISTS,
    LIBTMAN_ID_SWAP,

    LIBTMAN_COL_ADD,
    LIBTMAN_COL_MOVE,
    LIBTMAN_COL_DEL,
    LIBTMAN_COL_EXISTS,

    LIBTMAN_DIR_ID_MAKE,
    LIBTMAN_DIR_ID_DEL,
    LIBTMAN_DIR_ID_REN,
    LIBTMAN_UNIT_MAKE,
    LIBTMAN_UNIT_DEL,
    LIBTMAN_DIR_ID_OPEN,

    LIBTMAN_DIR_PRJ_DEL,
    LIBTMAN_DIR_PRJ_MAKE,
    LIBTMAN_DIR_PRJ_OPEN,

    LIBTMAN_UNIT_SET,
    LIBTMAN_UNIT_GET,
    LIBTMAN_UNIT_ILLEG,

    LIBTMAN_CMD_BIN,
    LIBTMAN_CMD_PGN,
    LIBTMAN_CMD_GEN,

    LIBTMAN_NODEF_ERR,

    __LIBTMAN_STATUS_LAST
};

struct tman_err_codes {
    enum tman_err_enum err;
    char desc[ERRMSGSIZ + 1];
};

int emod_get(void);
int emod_set(int err);
const char *emod_strerror(void);

#endif
