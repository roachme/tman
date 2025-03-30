#ifndef LIBTMAN_CLI_PLUGIN_H
#define LIBTMAN_CLI_PLUGIN_H

#include "cli.h"

int tman_cli_plugin(char *name, int argc, char **argv,
                    struct tman_context *ctx);

#endif
