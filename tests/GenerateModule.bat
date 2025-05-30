mkdir build
cd build

cmake ..
cmake --build .

cd..
copy "..\bin\lib\Debug\network.dll" ".\bin\Debug\"