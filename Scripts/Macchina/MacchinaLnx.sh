#!/bin/bash

wget https://github.com/macchina-io/macchina.io/archive/refs/tags/macchina-0.11.0-release.tar.gz
tar -xf macchina-0.11.0-release.tar.gz
rm -rf macchina-0.11.0-release.tar.gz
cd macchina.io-macchina-0.11.0-release
make -j8
sudo make install
cd ..
rm -rf macchina.io-macchina-0.11.0-release
#sudo ldconfig /usr/local/macchina/lib/
