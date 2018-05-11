#!/bin/bash
set -e
find src test experiment -name "*.cpp" -o -name "*.hpp" > gtags.files
gtags

