#include <stdio.h>
#include <string.h>
#include "board.h"
#include "common.h"

#define BRDSIZ      10          /* maximum size of project board name */

/* For external use. Used only by getters so no harm to module.  */
static char boardcurr[IDSIZ + 1], boardprev[IDSIZ + 1];

static int load(char *prj)
{
    return 1;
}

static char curr[BRDSIZ + 1], prev[BRDSIZ + 1];

char *board_curr(char *prj)
{
    if (load(prj) || curr[0] == '\0')
        return NULL;
    return strncpy(boardcurr, curr, IDSIZ);
}
