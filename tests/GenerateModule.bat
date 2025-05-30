mkdir build
cd build

cmake ..
cmake --build .

cd..
copy "..\bin\lib\Debug\jstypes.dll" ".\bin\Debug\"