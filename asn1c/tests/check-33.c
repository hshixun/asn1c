#undef	NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>

#include <T.h>

int
main(int ac, char **av) {
	T_t t;

	memset(&t, 0, sizeof(t));

	/*
	 * No plans to fill it up: just checking whether it compiles or not.
	 */

	return 0;
}
