#!/bin/sh

touch NEWS AUTHORS COPYING ChangeLog
autopoint \
&& aclocal \
&& automake --gnu --add-missing \
&& autoconf