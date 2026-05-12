/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "error.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==========================================================================================================
static char g_program_name[32];

// =============================================================================
ATTRIBUTE(format(printf, 1, 0))
static void
error_impl(const char* restrict fmt, va_list args)
{
	int saved_errno = errno;

	fflush(stdout);
	fprintf(stderr, "%s: ", program_name());
	vfprintf(stderr, fmt, args);

	if (saved_errno != 0) {
		fprintf(stderr, ": %s", strerror(saved_errno));
	}

	fputc('\n', stderr);
	fflush(stderr);
}

void
set_program_name(const char name[restrict static 1])
{
	strncpy(g_program_name, name, ARRAYSIZE(g_program_name) - 1);
}

const char*
program_name(void)
{
	return g_program_name;
}

void
error(const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	error_impl(fmt, ap);
	va_end(ap);
}

noreturn void
die(int status, const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	error_impl(fmt, ap);
	va_end(ap);
	exit(status);
}
