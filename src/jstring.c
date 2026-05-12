/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "jstring.h"

// =============================================================================
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
