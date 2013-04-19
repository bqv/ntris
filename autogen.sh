#!/bin/sh

#touch NEWS AUTHORS ChangeLog
aclocal \
&& automake --gnu --add-missing \
&& autoconf
