#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "config.h"

#define LP64  models[0]
#define ILP64 models[1]
#define LLP64 models[2]
#define ILP32 models[3]
#define LP32  models[4]

#define BITS(__n) (__n * 8)

const char *models[] = { "LP64", "ILP64", "LLP64", "ILP32", "LP32" };

struct sizes {
	size_t c;
	size_t s;
	size_t i;
	size_t l;
	size_t ll;
	size_t p;
};

struct sizes get_sizes()
{
	struct sizes szs = { .c = BITS(sizeof(char)),
			     .s = BITS(sizeof(short)),
			     .i = BITS(sizeof(int)),
			     .l = BITS(sizeof(long)),
			     .ll = BITS(sizeof(long long)),
			     .p = BITS(sizeof(void *)) };

	return szs;
}

void print_sizes(int addlabel, struct sizes *szs)
{
	if (addlabel)
		printf("%5s %5s %5s %5s %5s %5s\n", "char", "short", "int",
		       "long", "llong", "ptr");
	printf("%5zu %5zu %5zu %5zu %5zu %5zu\n", szs->c, szs->s, szs->i,
	       szs->l, szs->ll, szs->p);
}

void print_model(struct sizes *szs)
{
	const char *model = "unknown";

	if (szs->p == 64) {
		if (szs->l != 64)
			// LLP64 preserves I = L.
			model = LLP64;
		else if (szs->i == 64)
			model = ILP64;
		else
			model = LP64;
	} else {
		if (szs->i == 32)
			model = ILP32;
		else
			model = LP32;
	}

	printf("%s\n", model);
}

void print_usage(char *name)
{
	printf("Usage: %s [OPTIONS]...\n", name);
	printf("Prints the programming model of the host and basic type sizes.\n\n");
	printf("OPTIONS:\n");
	printf("   -h     prints this help message.\n");
	printf("   -v     prints the version.\n");
	printf("   -s     prints the sizes of types in the order of:\n");
	printf("             char short int long longlong pointer\n");
	printf("   -S     same as -s, but add a header of type names.\n");
	printf("   -m     prints the programming model.\n");
	printf("\nWithout any arguments, -m is assumed.\n");
}

void print_version()
{
	printf("%s\n", PACKAGE_STRING);
	printf("Report bus to %s or %s\n", PACKAGE_BUGREPORT, PACKAGE_URL);
	printf("Copyright 2021 Nisal Bandara\n");
}

int main(int argc, char **argv)
{
	struct sizes sizes = get_sizes();

	opterr = 0; // clear it

	int c = 0;
	while ((c = getopt(argc, argv, "hvsS")) != -1) {
		switch (c) {
		case 'h':
			print_usage(argv[0]);
			goto exit;
		case 'v':
			print_version();
			goto exit;
		case 's':
			print_sizes(0, &sizes);
			goto exit;
		case 'S':
			print_sizes(1, &sizes);
			goto exit;
		case 'm':
			goto noopt;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n",
					optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		default:
			abort();
		}
	}

noopt:
	print_model(&sizes);

exit:
	return 0;
}
