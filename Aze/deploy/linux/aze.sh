#!/bin/bash
# Get the full name of this file
me=$(readlink --canonicalize --no-newline $0)
# Get the directory of this file
base=$(dirname "$me")
# Make OS look for librairies in "./"
export LD_LIBRARY_PATH=$base
#export QT_DEBUG_PLUGINS=1
$base/aze $@
