#!/bin/sh

#touch NEWS AUTHORS COPYING ChangeLog
aclocal \
&& automake --gnu --add-missing \
&& autoconf
