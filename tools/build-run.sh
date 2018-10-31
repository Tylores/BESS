#!/bin/bash

# setup environment
export CPU=x86_64
export OS=linux
export VARIANT=debug

# modbus setup
export MBLIB=/usr/local/lib
export MBINC=/usr/local/include

# boost setup
export BOOSTINC=$HOME/src/boost_1_66_0

# build
export SRC=bess
make -C ../build

# run
./../build/bin/debug/$SRC -c ../data/config.ini
