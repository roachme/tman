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

    TMAN_EREQRENV,
    TMAN_EMISSENV,
    TMAN_EILLEGENV,
    TMAN_ENOCURRENV,
    TMAN_ENOPREVENV,
    TMAN_ENOSUCHENV,
    TMAN_EENVEXISTS,

    TMAN_EREQRID,
    TMAN_EMISSID,
    TMAN_EADDID,
    TMAN_EDELID,
    TMAN_EILLEGID,
    TMAN_ENOCURRID,
    TMAN_ENOPREVID,
    TMAN_ENOSUCHID,
    TMAN_EIDEXISTS,
    TMAN_ESWAPIDS,

    TMAN_ETASKMKDIR,
    TMAN_ETASKRMDIR,
    TMAN_ETASKMKUNIT,
    TMAN_ETASKRMUNIT,
    TMAN_ECOLNEXIST,

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
