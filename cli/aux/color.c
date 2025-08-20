#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "color.h"
#include "config.h"

/* Got this module from GNU grep */

static const char *sgr_start = "\33[%sm\33[K";
static const char *sgr_end = "\33[m\33[K";

/* Start a colorized text attribute on stdout using the SGR_START
   format; the attribute is specified by SGR_SEQ.  */
static void print_start_colorize(char const *_sgr_start, char const *sgr_seq)
{
    printf(_sgr_start, sgr_seq);
}

/* Restore the normal text attribute using the SGR_END string.  */
static void print_end_colorize(char const *_sgr_end)
{
    fputs(_sgr_end, stdout);
}

/* SGR utility functions.  */
/* NOTE: pro'ly variable 's' is a color string.  */
static void pr_sgr_start(char const *s)
{
    if (*s)
        print_start_colorize(sgr_start, s);
}

static void pr_sgr_end(char const *s)
{
    if (*s)
        print_end_colorize(sgr_end);
}

void color_print_str(const char *fmt, char *str, char *color)
{
    if (isatty(STDOUT_FILENO) && teccfg.opts.color == true)
        pr_sgr_start(color);
    printf(fmt, str);
    if (isatty(STDOUT_FILENO) && teccfg.opts.color == true)
        pr_sgr_end(CRESET);
}

/*
void color_print_char(const char *fmt, char c, char *color)
{
    if (isatty(STDOUT_FILENO) && teccfg.opts.color == true)
        pr_sgr_start(color);
    printf(fmt, c);
    if (isatty(STDOUT_FILENO) && teccfg.opts.color == true)
        pr_sgr_end(CRESET);
}
*/
