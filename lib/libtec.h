#ifndef LIBTEC_H
#define LIBTEC_H

#define TEC_ERRMSGSIZ   100

typedef struct tec_arg tec_arg_t;
typedef struct tec_unit tec_unit_t;
typedef struct tec_list tec_list_t;
typedef struct tec_context tec_ctx_t;

struct tec_arg {
    char *project;
    char *board;
    char *taskid;
};

struct tec_unit {
    char *key;
    char *val;
    struct tec_unit *next;
};

struct tec_list {
    char *name;
    int status;                 /* Status code of the object */
    struct tec_list *next;
};

struct tec_context {
    struct tec_unit *units;
    struct tec_unit *column;
    struct tec_list *list;
};

enum tec_errno {
    LIBTEC_OK,                  /* OK, interpret output as path or output */

    LIBTEC_SYS_DB,
    LIBTEC_SYS_MALLOC,

    LIBTEC_ARG_ILLEG,
    LIBTEC_ARG_NOSUCH,
    LIBTEC_ARG_EXISTS,

    LIBTEC_DIR_DEL,
    LIBTEC_DIR_MAKE,
    LIBTEC_DIR_MOVE,
    LIBTEC_DIR_OPEN,
    LIBTEC_DIR_RENAME,

    LIBTEC_UNIT_ADD,
    LIBTEC_UNIT_DEL,
    LIBTEC_UNIT_GET,
    LIBTEC_UNIT_ILLEG,
    LIBTEC_UNIT_KEY,
    LIBTEC_UNIT_LOAD,
    LIBTEC_UNIT_SAVE,
    LIBTEC_UNIT_SET,

    /* HOTFIX: delete it once error code table is filled.  */
    LIBTEC_NODEF_ERR,

    __LIBTEC_STATUS_LAST
};

struct tec_err_codes {
    enum tec_errno err;
    char desc[TEC_ERRMSGSIZ + 1];
};

/* Core functions.  */
int tec_make_db(const char *taskdir);
int tec_check_db(const char *taskdir);
char *tec_strerror(int errnum);

/* Data structure for unit values.  */
tec_unit_t *tec_unit_add(tec_unit_t * head, char *key, char *val);
tec_unit_t *tec_unit_parse(struct tec_unit *head, const char *str);
char *tec_unit_key(tec_unit_t * head, char *key);
void *tec_unit_free(tec_unit_t * units);
int tec_unit_set(struct tec_unit *head, char *key, char *val);
int tec_unit_save(const char *filename, tec_unit_t * units);

/* Data structure for list of objects.  */
void *tec_list_free(tec_list_t * list);

/* Input argument functions.  */
int tec_project_exist(const char *taskdir, tec_arg_t * args);
int tec_project_valid(const char *taskdir, tec_arg_t * args);
int tec_board_exist(const char *taskdir, tec_arg_t * args);
int tec_board_valid(const char *taskdir, tec_arg_t * args);
int tec_task_exist(const char *taskdir, tec_arg_t * args);
int tec_task_valid(const char *taskdir, tec_arg_t * args);

/* Task functions.  */
int tec_task_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_move(const char *taskdir, tec_arg_t * src, tec_arg_t * dst,
                  tec_ctx_t * ctx);
int tec_task_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_column_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_task_column_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);

/* Board functions.  */
int tec_board_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_move(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_board_column_set(const char *taskdir, tec_arg_t * args,
                         tec_ctx_t * ctx);
int tec_board_column_get(const char *taskdir, tec_arg_t * args,
                         tec_ctx_t * ctx);

/* Project functions.  */
int tec_project_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_project_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_project_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_project_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_project_rename(const char *taskdir, tec_arg_t * src, tec_arg_t * dst,
                       tec_ctx_t * ctx);
int tec_project_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx);
int tec_project_column_set(const char *taskdir, tec_arg_t * args,
                           tec_ctx_t * ctx);
int tec_project_column_get(const char *taskdir, tec_arg_t * args,
                           tec_ctx_t * ctx);

#endif
