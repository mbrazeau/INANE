#!/bin/bash
cd ./external/ncl
pwd
mkdir build
cd build
cmake ..
make -j 2
cd ../../..
pwd
mkdir build
cd build
qmake ..
make
macdeployqt INANE.app -appstore-compliant
cd ..
cp ./external/ncl/build/ncl/libncl.dylib ./build/INANE.app/Contents/Frameworks

