#!/usr/bin/env sh
set -e

# ================================================================================
prog=$1
shift

tmpf=$(mktemp "tmp.XXXXXXXX") || exit 1
trap 'rm -f "$tmpf"' EXIT

while test $# -gt 0; do
	source=${1}
	name=${source##*/}
	name=${name%.json}
	printf 'json_tests::%s ... ' "$name"
	"$prog" "$source" | sed 's|\r$||' >"$tmpf"
	if result=$(diff -u "$tmpf" "${source}l"); then
		printf "\033[32mok\033[0m\n"
	else
		printf "\033[31mfail\033[0m\n"
		printf '%s\n' "$result" >&2
	fi
	shift
done
