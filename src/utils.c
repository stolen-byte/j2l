/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "utils.h"
#include "jstring.h"

#include <errno.h>
#include <math.h>

// =============================================================================
static inline const char*
trim(const char p[static 1])
{
	while (*p && is_space(*p))
		++p;
	return p;
}

static inline int
shift_for_suffix(const char suffix[static 1])
{
	static const char sizes[] = "bkmg";
	int result = 0;

	suffix = trim(suffix);
	errno = 0;

	const char* ptr = strchr(sizes, to_lower(*suffix++));
	if (!ptr) //
		goto invalid;

	result = 10 * (int)((size_t)ptr - (size_t)sizes);

	// 'b' is optional at the end, except for 'bb', which is invalid
	if (result > 0 && to_lower(*suffix) == 'b') //
		suffix++;

	suffix = trim(suffix);
	if (*suffix != 0) //
		goto invalid;

	return result;

invalid:
	errno = EILSEQ;
	return 0;
}

size_t
parse_size(const char str[static 1])
{
	str = trim(str);
	if (*str == 0) {
		errno = EINVAL;
		return 0;
	}

	// negative value
	if (*str == '-') {
		errno = ERANGE;
		return 0;
	}

	errno = 0;
	char* endp = NULL;
	uintmax_t iv = strtoumax(str, &endp, 10);
	double value;

	// try integer parse first, because simply using `strtod` when supplied a large enough int
	// will lose precision (mantissa-limited).
	// although we have to explicitly check for '.'
	if (errno == 0 && endp != str && *endp != '.') {
		value = (double)iv;
	} else {
		if (errno == ERANGE) return 0;

		// try float
		errno = 0;
		endp = NULL;
		value = strtod(str, &endp);

		if (endp == str) errno = EINVAL;
		if (errno != 0) return 0;
	}

	if (endp != str && *endp != 0) {
		// parse suffix
		int shift = shift_for_suffix(endp);
		if (errno != 0) return 0;

		if (shift > 0) {
			value = ldexp(value, shift);
		}
	}

	if (!isfinite(value) || (value < 0.0) || (value > (double)SIZE_MAX)) {
		errno = ERANGE;
		return 0;
	}

	// round/llrint not necessary
	return (size_t)value;
}
