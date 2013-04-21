#!/bin/sh

echo OSX not supported.
echo Building for Windows...
sh -x win32-build.sh
echo Building for Linux...
sh -x linux-build.sh
