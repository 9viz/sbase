/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>

#include <errno.h>
#include <stdio.h>

#include "fs.h"
#include "util.h"

static int mv_status = 0;
static int verbose   = 0;
static int ask       = 0;

static int
mv(const char *s1, const char *s2, int depth)
{
    struct recursor r = { .fn = rm, .hist = NULL, .depth = 0, .maxdepth = 0,
                          .follow = 'P', .flags = 0 };
    if (verbose)
        fprintf(stderr, "'%s' -> '%s'\n", s1, s2);
    if (ask) {
        struct stat st;
        if (stat(s1, &st) == 0 && stat(s2, &st) == 0) {
            printf("mv: overwrite '%s'? ", s2);
            char ow = getchar();
            if (ow != 'y')
                return -1;
        }
    }
    if (!rename(s1, s2))
        return (mv_status = 0);
    if (errno == EXDEV) {
        cp_aflag  = cp_rflag = cp_pflag = 1;
        cp_follow = 'P';
        cp(s1, s2, depth);
        recurse(s1, NULL, &r);
        return (mv_status = cp_status || rm_status);
    }
    mv_status = 1;

    return -1;
}

static void
usage(void)
{
    eprintf("usage: %s [-fvi] source ... dest\n", argv0);
}

int
main(int argc, char *argv[])
{
    struct stat st;

    ARGBEGIN {
    case 'f':
        ask = 0;
        break;
    case 'v':
        verbose = 1;
        break;
    case 'i':
        ask = 1;
        break;
    default:
        usage();
    } ARGEND

    if (argc < 2)
        usage();

    if (argc > 2) {
        if (stat(argv[argc - 1], &st) < 0)
            eprintf("stat %s:", argv[argc - 1]);
        if (!S_ISDIR(st.st_mode))
            eprintf("%s: not a directory\n", argv[argc - 1]);
    }
    enmasse(argc, argv, mv);

    return mv_status;
}
