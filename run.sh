#!/bin/bash

make clean
make
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(realpath lib)
./bin/main