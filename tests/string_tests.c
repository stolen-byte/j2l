/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "jstring.h"
#include "testing.h"

// =============================================================================
TEST(jstrlcpy)
{
	const char* reg = "testing";
	const size_t rlen = strlen(reg);

	char buf[rlen + 1];
	memset(buf, 'X', sizeof(buf));

	// should return length of string minus null terminator
	size_t copied = jstrlcpy(sizeof(buf), buf, reg);
	require_streq(buf, reg);
	require(copied == rlen);

	if (buf[rlen] != '\0') {
		fail("copied string was not null terminated");
	}

	// source smaller than dest
	const char* small = "abc";
	const size_t slen = strlen(small);
	copied = jstrlcpy(sizeof(buf), buf, small);
	require_streq(buf, small);
	require(copied == slen);
	if (buf[slen] != '\0') {
		fail("copied string was not null terminated");
	}

	const char* big = "i am a test string";
	const size_t blen = strlen(big);
	copied = jstrlcpy(sizeof(buf), buf, big);
	require_streq(buf, "i am a ");

	if (copied != blen) {
		fail("did not return the length of src");
	}

	if (buf[rlen] != '\0') {
		fail("copied string was not null terminated");
	}
}

// =============================================================================
DECLARE_TESTS("string_tests", jstrlcpy)
