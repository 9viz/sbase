/* See LICENSE file for copyright and license details. */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf.h"
#include "util.h"

static char *format = "";

static void
strings(FILE *fp, const char *fname, size_t len)
{
    Rune r, *rbuf;
    size_t i, bread;
    off_t off;

    rbuf = ereallocarray(NULL, len, sizeof(*rbuf));

    for (off = 0, i = 0; (bread = efgetrune(&r, fp, fname)); ) {
        off += bread;
        if (r == Runeerror)
            continue;
        if (!isprintrune(r)) {
            if (i > len)
                putchar('\n');
            i = 0;
            continue;
        }
        if (i < len) {
            rbuf[i++] = r;
            continue;
        } else if (i > len) {
            efputrune(&r, stdout, "<stdout>");
            continue;
        }
        printf(format, (long)off - i);
        for (i = 0; i < len; i++)
            efputrune(rbuf + i, stdout, "<stdout>");
        efputrune(&r, stdout, "<stdout>");
        i++;
    }
    free(rbuf);
}

static void
usage(void)
{
    eprintf("usage: %s [-a] [-n num] [-t format] [file ...]\n", argv0);
}

int
main(int argc, char *argv[])
{
    FILE *fp;
    size_t len = 4;
    int ret = 0;
    char f;

    ARGBEGIN {
    case 'a':
        break;
    case 'n':
        len = estrtonum(EARGF(usage()), 1, LLONG_MAX);
        break;
    case 't':
        format = estrdup("%8l#: ");
        f = *EARGF(usage());
        if (f == 'd' || f == 'o' || f == 'x')
            format[3] = f;
        else
            usage();
        break;
    default:
        usage();
    } ARGEND

    if (!argc) {
        strings(stdin, "<stdin>", len);
    } else {
        for (; *argv; argc--, argv++) {
            if (!strcmp(*argv, "-")) {
                *argv = "<stdin>";
                fp = stdin;
            } else if (!(fp = fopen(*argv, "r"))) {
                weprintf("fopen %s:", *argv);
                ret = 1;
                continue;
            }
            strings(fp, *argv, len);
            if (fp != stdin && fshut(fp, *argv))
                ret = 1;
        }
    }

    ret |= fshut(stdin, "<stdin>") | fshut(stdout, "<stdout>");

    return ret;
}
