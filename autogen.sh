#!/bin/sh

aclocal
automake --add-missing --foreign --copy
autoconf
