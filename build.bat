@echo off
rmdir /Q /S build
mkdir build
cd build

cmake ..
cmake --build . --config Release

cd ..