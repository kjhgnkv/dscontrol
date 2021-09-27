#!/bin/bash

git clone https://github.com/madler/zlib
cd zlib
cmake .
cmake --build . -- -j${nproc}
sudo cmake --build . --target install
cd ..
rm -rf zlib
