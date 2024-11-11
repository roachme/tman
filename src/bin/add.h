#ifndef ADD_H
#define ADD_H

enum {
    ADD_OK = 0,
    ADD_EXIST,
};

struct tman_add_opts {
};

int tman_add(int argc, char **argv);

#endif
