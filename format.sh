#!/bin/bash
echo formating...
find ./src -name "*.cpp" -o -name "*.hpp"  | xargs clang-format -i
find ./test -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
