mkdir -p ./build
cd build && cmake -DCMAKE_C_COMPILER=clang .. && cmake --build .
cd ..
