#!/bin/bash
config=${1:-debug}
filter=${2:-*}

python configure.py
echo make utest config=${config}
make utest config=${config}
RESULT=$?
[ $RESULT -ne 0 ] && exit 1

echo ./build/${config}/utest --gtest_filter="$filter"
./build/${config}/utest --gtest_filter="$filter"

RESULT=$?
[ $RESULT -ne 0 ] && exit 1
exit 0
