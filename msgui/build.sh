set -xe

CXX=$(which g++-14) cmake -B ../build_dir -GNinja .
cmake --build ../build_dir -- -j$(nproc)
mkdir -p ../lib_out
mv ../build_dir/libmsgui.so ../lib_out
# rm -rf out_debug

