#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#include "asn1parser.h"
#include "asn1p_list.h"

int asn1p_parse(void **param);

void *asn1p__scan_bytes(const char *, int len);
void *asn1p__delete_buffer(void *);
void *asn1p_restart(FILE *);

extern int asn1p_lineno;

static int _asn1p_set_flags(enum asn1p_flags flags);
static int _asn1p_assign_filename(asn1p_t *a, const char *fname);

/*
 * Parse the given buffer.
 */
asn1p_t *
asn1p_parse_buffer(const char *buffer, int size /* = -1 */, enum asn1p_flags flags) {
	asn1p_t *a = 0;
	void *ap;
	void *ybuf;
	int ret;

	if(_asn1p_set_flags(flags)) {
		/* EINVAL */
		return 0;
	}

	if(size < 0)
		size = strlen(buffer);

	ybuf = asn1p__scan_bytes(buffer, size);
	if(!ybuf) {
		assert(ybuf);
		return 0;
	}

	asn1p_lineno = 1;

	ap = (void *)&a;
	ret = asn1p_parse(ap);

	asn1p__delete_buffer(ybuf);

	if(ret == 0) {
		assert(a);
		if(_asn1p_assign_filename(a, "-"))
			return NULL;	/* FIXME: destroy (a) */
	} else {
		assert(a == NULL);
	}

	return a;
}


/*
 * Parse the file identified by its name.
 */
asn1p_t *
asn1p_parse_file(const char *filename, enum asn1p_flags flags) {
	struct stat sb;
	asn1p_t *a = 0;
	void *ap;
	FILE *fp;
	int ret;

	if(_asn1p_set_flags(flags)) {
		/* EINVAL */
		return 0;
	}

	fp = fopen(filename, "r");
	if(fp == NULL) {
		perror(filename);
		return NULL;
	}

	if(fstat(fileno(fp), &sb)
	|| !S_ISREG(sb.st_mode)) {
		fclose(fp);
		fprintf(stderr, "%s file not recognized: Bad file format\n",
			filename);
		errno = EINVAL;
		return NULL;
	}

	asn1p_lineno = 1;

	asn1p_restart(fp);

	ap = (void *)&a;
	ret = asn1p_parse(ap);

	fclose(fp);

	if(ret == 0) {
		assert(a);
		if(_asn1p_assign_filename(a, filename))
			return NULL;	/* FIXME: destroy (a) */
	} else {
		assert(a == NULL);
	}

	return a;
}

extern int asn1p_lexer_types_year;
extern int asn1p_lexer_constructs_year;
extern int asn1p__flex_debug;

static int
_asn1p_set_flags(enum asn1p_flags flags) {

	asn1p_lexer_types_year = 0;
	asn1p_lexer_constructs_year = 0;
	asn1p__flex_debug = 0;

	/*
	 * Enable debugging in lexer.
	 */
	if(flags & A1P_LEXER_DEBUG) {
		flags &= ~A1P_LEXER_DEBUG;
		asn1p__flex_debug = 1;
	}

	/*
	 * Restrict embedded types to ASN.1:1988 version of standard.
	 */
	if(flags & A1P_TYPES_RESTRICT_TO_1988) {
		flags &= ~A1P_TYPES_RESTRICT_TO_1988;
		asn1p_lexer_types_year = 1988;
	}

	/*
	 * Restrict embedded types to ASN.1:1988 version of standard.
	 */
	if(flags & A1P_TYPES_RESTRICT_TO_1988) {
		flags &= ~A1P_TYPES_RESTRICT_TO_1988;
		asn1p_lexer_types_year = 1988;
	}

	/*
	 * Check that we haven't missed an unknown flag.
	 */
	if(flags) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

static int
_asn1p_assign_filename(asn1p_t *a, const char *fname) {
	asn1p_module_t *mod;
	TQ_FOR(mod, &(a->modules), mod_next) {
		mod->source_file_name = strdup(fname);
		if(mod->source_file_name == NULL)
			return -1;
	}
	return 0;
}
