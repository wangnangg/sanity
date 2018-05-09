#!/bin/bash
set -e
find src test -name "*.cpp" -o -name "*.hpp" > gtags.files
gtags

