#include <string.h>

#include "cli.h"
#include "column.h"

// TODO: delete it all once support for user defined columns added.

struct column g_colname[] = {
    {.tag = "uknw",.mark = '?',.desc = "unknown tag"},
    {.tag = "curr",.mark = '*',.desc = "unknown tag"},
    {.tag = "prev",.mark = '^',.desc = "unknown tag"},
    {.tag = "blog",.mark = '+',.desc = "unknown tag"},
    {.tag = "done",.mark = '-',.desc = "unknown tag"},
    {.tag = "revw",.mark = '>',.desc = "review column"},
    {.tag = "test",.mark = '$',.desc = "test column"},
    {.tag = "lock",.mark = '!',.desc = "locked column"},
};

//#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
const int g_colname_size = sizeof(g_colname) / sizeof(g_colname[0]);

int column_exist(char *name)
{
    for (int k = 0; k < g_colname_size; ++k) {
        if (strcmp(g_colname[k].tag, name) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
