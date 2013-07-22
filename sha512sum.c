/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "util.h"
#include "crypt.h"
#include "sha512.h"

struct sha512 s;
struct crypt_ops sha512_ops = {
	sha512_init,
	sha512_update,
	sha512_sum,
	&s,
};

static void
usage(void)
{
	eprintf("usage: %s [file...]\n", argv0);
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	uint8_t md[SHA512_DIGEST_LENGTH];

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if (argc == 0) {
		cryptsum(&sha512_ops, stdin, "<stdin>", md);
		mdprint(md, "<stdin>", sizeof(md));
	} else {
		for (; argc > 0; argc--) {
			if ((fp = fopen(*argv, "r")) == NULL)
				eprintf("fopen %s:", *argv);
			cryptsum(&sha512_ops, fp, *argv, md);
			mdprint(md, *argv, sizeof(md));
			fclose(fp);
			argv++;
		}
	}

	return 0;
}
