git clone https://github.com/madler/zlib
cd zlib
cmake .
cmake --build . -- -j%NUMBER_OF_PROCESSORS%
cmake --build . --target install
cd ..
rmdir /Q /S zlib
