#!/bin/sh

INDENT_FILE=.indent.pro
test -f $INDENT_FILE || { echo "Indent profile not found for this project: $PWD"; exit 1; }
# XXX This is not an ideal assumption for development environments
test -x /usr/bin/indent || { echo "Command 'indent' was not found in the path."; exit 1; }

set -x
find . -name '*.c' -or -name '*.h' |
        xargs -I% -- indent %
rm -f *.BAK
