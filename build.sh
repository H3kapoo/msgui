set -xe

cd core
./build.sh
cd ..

CXX=$(which g++-14) cmake -B artifacts -GNinja .
cmake --build artifacts -j8
mv debug/gui .
rm -rf debug

