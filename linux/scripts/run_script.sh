#!/bin/sh

here="${0%/*}"

LD_LIBRARY_PATH="$here"/lib:"$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
exec "$here"/bin/qmk_toolbox "$@"