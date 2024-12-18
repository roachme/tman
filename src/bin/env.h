#ifndef ENV_H
#define ENV_H

struct tman_env_add_opt {
    int force;
};

struct tman_env_cat_opt {
    int force;
};

struct tman_env_del_opt {
    int force;
};

struct tman_env_list_opt {
    int force;
};

struct tman_env_prev_opt {
    int force;
};

struct tman_env_set_opt {
    int force;
};

struct tman_env_use_opt {
    int force;
};

int _env_add(int argc, char **argv);
int _env_cat(int argc, char **argv); // under consideration
int _env_del(int argc, char **argv);
int _env_list(int argc, char **argv);
int _env_prev(int argc, char **argv);
int _env_set(int argc, char **argv);
int _env_use(int argc, char **argv);

#endif
