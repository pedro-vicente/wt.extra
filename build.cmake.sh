#!/bin/bash
set -e
path_wt="$(pwd)/install/wt"
path_boost="$(pwd)/build/boost_1_88_0"
echo "Wt at: $path_wt"
echo "Boost at: $path_boost"
sleep 1

mkdir -p build/wt.extra
pushd build
pushd wt.extra

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake ../.. --fresh \
    -DWT_INCLUDE="$path_wt/include" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib" 
cmake --build . --parallel 2


elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake ../.. --fresh \
    -DWT_INCLUDE="$path_wt/include" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib" 
cmake --build . --parallel 

elif [[ "$OSTYPE" == "msys" ]]; then

cmake ../.. --fresh \
    -DWT_INCLUDE="$path_wt/include" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib"
cmake --build .  --config Debug --parallel 

fi

if [[ "$OSTYPE" != "msys" ]]; then
echo $(pwd)
echo "open browser http://localhost:9000"
./map --http-address=0.0.0.0 --http-port=9001  --docroot=. -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson
fi

popd
popd

