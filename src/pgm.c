#include <stdio.h>
#include <dirent.h>
#include "pgm.h"
#include "osdep.h"


int _pgn_chk(int argc, char **argv)
{
    printf("pgn: check installed plugins\n");
    return 1;
}

int _pgn_list(int argc, char **argv)
{
    printf("pgn: list installed plugins\n");
    return 1;
}
