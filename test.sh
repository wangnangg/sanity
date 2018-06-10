#!/bin/bash
set -e
config=${2:-debug}
filter=${1:-*}

python configure.py
echo make utest config=${config}
make utest config=${config}

echo ./build/${config}/utest --gtest_filter="$filter"
./build/${config}/utest --gtest_filter="$filter"
