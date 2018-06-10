#! /bin/bash

set -e

rm covhtml -rf

config=coverage
filter=${1:-*}

python configure.py
echo make utest config=${config}
make utest config=${config}

echo ./build/${config}/utest --gtest_filter="$filter"
./build/${config}/utest --gtest_filter="$filter"

mkdir -p covhtml
lcov -c -d . -b . --no-external -o covhtml/trace.info
genhtml covhtml/trace.info -o covhtml

echo start a browser from covhtml/index.html to view test coverage

