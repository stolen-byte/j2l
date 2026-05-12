/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "jstring.h"

// =============================================================================
static inline bool
is_separator(char ch)
{
#ifdef PLATFORM_WINDOWS
	return (ch == '/' || ch == '\\');
#else
	return ch == '/';
#endif
}

const char*
jbasename(char* path)
{
	if (!path || !*path) return ".";

	size_t i = strlen(path) - 1;
	for (; i && is_separator(path[i]); --i)
		path[i] = 0;

	while (i && !is_separator(path[i - 1]))
		--i;

	return path + i;
}

size_t
jstrlcpy(size_t size, char dst[restrict size], const char src[restrict static 1])
{
	size_t copied = 0;

	while (++copied < size && *src) {
		*dst++ = *src++;
	}

	if (copied <= size) {
		*dst = '\0';
	}

	return copied + strlen(src) - 1;
}
