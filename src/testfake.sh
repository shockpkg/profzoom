#!/bin/sh

self_dir="`dirname "$0"`"
cd "$self_dir"

export LD_PRELOAD="$LD_PRELOAD:./profzoom.so"
export PROFZOOM_PREFIX='/someprefix/'

rm -rf './testfake-data'
mkdir './testfake-data'
printf '%s' 'readdata' > './testfake-data/data.txt'

unset NOT_EXIST
export IS_EMPTY=''
export IS_SET='SET'
./testfake
