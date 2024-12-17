#ifndef LIST_H
#define LIST_H

/*
   -A - list all (even done tasks)
   -a - almost all (everything but done tasks)
   -c - specify what column to list
   -s - default: list only current & previous (maybe?)
   */
struct tman_list {
    int all;
    int almost;
    char *col;
    int spec;
    int help;
};

#endif
