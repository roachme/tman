#include <ctype.h>

#include "valid.h"

/* TODO: Maybe there's generic function to validate name.
 *       If so unify them all.  */

int valid_column_name(char *name)
{
    if (!isalnum(*name++))
        return 0;
    for (; *name; ++name)
        if (!(isalnum(*name) || *name == '_' || *name == '-'))
            return 0;
    return isalnum(*--name) != 0;
}

int valid_project_name(char *name)
{
    if (!isalnum(*name++))
        return 0;
    for (; *name; ++name)
        if (!(isalnum(*name) || *name == '_' || *name == '-'))
            return 0;
    return isalnum(*--name) != 0;
}

int valid_board_name(char *name)
{
    if (!isalnum(*name++))
        return 0;
    for (; *name; ++name)
        if (!(isalnum(*name) || *name == '_' || *name == '-'))
            return 0;
    return isalnum(*--name) != 0;
}

int valid_task_name(char *name)
{
    if (!isalnum(*name++))
        return 0;
    for (; *name; ++name)
        if (!(isalnum(*name) || *name == '_' || *name == '-'))
            return 0;
    return isalnum(*--name) != 0;
}
