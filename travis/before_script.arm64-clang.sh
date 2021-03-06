#!/bin/bash
set -ev
mkdir sleef.build
cd sleef.build
export CC=clang-8
export CXX=clang++-8
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install -DSLEEF_SHOW_CONFIG=1 -DENFORCE_TESTER3=TRUE -DBUILD_QUAD=TRUE -DENABLE_ALTDIV=TRUE -DENABLE_ALTSQRT=TRUE ..
