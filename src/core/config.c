#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

static char *fconfig = NULL;
static char *dconfig = NULL;

/*
    CONFIG = ~/.config/tman/sys.conf
    DCONFIG = ~/.config/tman
    HOOKLS = "list tag author"
    HOOKLS = "list tag author"
*/

int findconfig()
{
    char homedir[BUFSIZ];
    sprintf(homedir, "%s", getenv("HOME"));
    char *configs[] = {};
    return 0;
}
