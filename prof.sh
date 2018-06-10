#! /bin/bash

set -e

config=profile
filter=${1:-*}

python configure.py
echo make exp config=${config}
make exp config=${config}

echo ./build/${config}/exp --gtest_filter="$filter"
./build/${config}/exp --gtest_filter="$filter"

gprof build/profile/exp gmon.out > prof_result

echo check the file prof_result

