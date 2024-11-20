#!/bin/sh

set -xe

CC=clang
CFLAGS="-g -Wall -Wextra -Wpedantic"

CFILES="$(find src/ -name "*.c")"

rm -rf build/ obj/
mkdir build/ obj/

for f in $CFILES; do
	OBJNAME="$(echo $f | sed -e "s/src/obj/;s/\.c/\.o/")"
	OBJS="$OBJS $OBJNAME"
	$CC $CFLAGS -c $f -o $OBJNAME
done

$CC $OBJS -o build/asdf
