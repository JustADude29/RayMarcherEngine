#!/bin/sh
mkdir -p build && cd build

cmake .. -DCMAKE_INSTALL_PREFIX=${PWD}/install_dir -DCMAKE_EXPORT_COMPILE_COMMANDS=1

