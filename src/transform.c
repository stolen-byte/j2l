/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "transform.h"
#include "jstring.h"

#include <ctype.h>

// =============================================================================
size_t
transform_next(transform_ctx ctx[restrict static 1],
               size_t size,
               const char in[restrict size],
               char out[restrict size * 2])
{
	const char* end = in + size;
	char* oout = out;

	while (in != end) {
		char ch = *in++;

		if (isspace((int)ch)) continue;

		*out++ = ch;
		if (ch == '{') {
			ctx->level++;
		} else if (ch == '}') {
			if (ctx->level > 0) ctx->level--;
			if (ctx->level == 0) *out++ = '\n';
		}
	}

	return (size_t)(out - oout);
}
