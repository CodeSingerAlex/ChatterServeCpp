# !/bin/bash

set -x

rm -rf `pwd`/build

mkdir `pwd`/build

cd `pwd`/build

cmake .. && make