#!/bin/bash
filter=${1:-*}
config=${2:-debug}

python ./configure.py
echo make exp config=${config}
make exp config=${config}
RESULT=$?
[ $RESULT -ne 0 ] && exit 1

echo ./build/${config}/exp --gtest_filter="$filter" ${@:3}
./build/${config}/exp --gtest_filter="$filter" ${@:3}

RESULT=$?
[ $RESULT -ne 0 ] && exit 1
exit 0
