#!/bin/sh

PROG="example_ecjp_3"
BASE_DIR=".."
#echo Work dir = $BASE_DIR
BUILD_DIR="${BASE_DIR}/build"
TESTS_DIR="${BASE_DIR}/tests"
LD_LIBRARY_PATH=$BUILD_DIR

#echo BUILD_DIR = $BUILD_DIR
#echo TESTS_DIR = $TESTS_DIR

export LD_LIBRARY_PATH
./test_all.sh ${BUILD_DIR}/${PROG} ${TESTS_DIR}
