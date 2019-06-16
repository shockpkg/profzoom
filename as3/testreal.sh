#!/bin/sh

if [ "$#" -ne 1 ]; then
	echo "usage: $0 <flashplayerbinpath>"
	exit 1
fi

self_dir="`dirname "$0"`"
cd "$self_dir"

export LD_PRELOAD="$LD_PRELOAD:./profzoom.so"
export PROFZOOM_PREFIX='/someprefix/'

rm -rf './testreal-data'
mkdir './testreal-data'
printf '%s' 'readdata' > './testreal-data/data.txt'

unset NOT_EXIST
export IS_EMPTY=''
export IS_SET='SET'
"$1" ./testreal.swf
