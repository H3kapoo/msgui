set -xe

CXX=$(which g++-14) cmake -B build_dir -GNinja .
cmake --build build_dir -j8
mv build_dir/libcore.so lib
# rm -rf out_debug

