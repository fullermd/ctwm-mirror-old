#!/bin/sh
# This spits to stdout; we expect to be redirected.

src=$1

# The header
echo '/*'
echo ' * AUTOGENERATED FILE -- DO NOT EDIT'
echo ' * This file is generated automatically from the default'
echo ' * ctwm bindings file system.ctwmrc by the build process'
echo ' */'
echo
echo '#include <stddef.h> // for NULL'
echo '#include "deftwmrc.h"'
echo

# We define one big char* arrray of the lines
echo 'const char *defTwmrc[] = {'

sed                      \
	-e 's/"/\\"/g'       \
	-e 's/^/    \"/'     \
	-e 's/$/",/'         \
	${src}

echo '    NULL'
echo '};'
