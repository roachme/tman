#ifndef ENV_H
#define ENV_H

int _env_add(int argc, char **argv);
int _env_del(int argc, char **argv);
int _env_list(int argc, char **argv);
int _env_prev(int argc, char **argv);
int _env_set(int argc, char **argv);
int _env_use(int argc, char **argv);

#endif