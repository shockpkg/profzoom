#!/bin/sh

self_dir="`dirname "$0"`"
cd "$self_dir"

rm -rf './testunit-data'
mkdir './testunit-data'
printf '%s' 'readdata' > './testunit-data/data.txt'

unset NOT_EXIST
export IS_EMPTY=''
export IS_SET='SET'
./testunit
