#ifndef LIBTEC_ERRMOD_H
#define LIBTEC_ERRMOD_H

#define ERRMSGSIZ   100
#define NERRCODE    50

int emod_set(int err);
char *emod_strerror(int errnum);

#endif
