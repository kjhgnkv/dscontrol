#!/bin/sh

cd $(dirname $0)/build
cmake --build . && /usr/local/macchina/bin/bundle ../Bundle_v1.bndlspec
cmake --build . && /usr/local/macchina/bin/bundle ../Bundle_v2.bndlspec

