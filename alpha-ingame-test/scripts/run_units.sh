#!/bin/bash
set -e
cd ./build/Debug/tests
CPU_CORES=$(getconf _NPROCESSORS_ONLN)
ctest -j $CPU_CORES
