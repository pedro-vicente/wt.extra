#!/bin/bash

path_wt="$(pwd)/ext/wt-4.12.0"
path_wt_build="$(pwd)/build/wt-4.12.0"
path_boost="$(pwd)/build/boost_1_88_0"
echo "Wt at: $path_wt"
echo "Wt build at: $path_wt_build"
echo "Boost at: $path_boost"

mkdir -p build/wt.extra
pushd build
pushd wt.extra

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake ../.. -DWT_INCLUDE=/home/pvn/wt_install/include -DWT_CONFIG_H=/home/pvn/wt_install/include

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake ../.. -DWT_INCLUDE=/Users/pvn/wt_install/include -DWT_CONFIG_H=/Users/pvn/wt_install/include

elif [[ "$OSTYPE" == "msys" ]]; then

cmake ../.. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib"

fi
cmake --build . --parallel 
popd
pwd

if [[ "$OSTYPE" != "msys" ]]; then
pushd build
export LD_LIBRARY_PATH=/home/pvn/wt.extra/build/boost_1_88_0/lib:$LD_LIBRARY_PATH
echo "open browser http://localhost:8080"
./test_leaflet --http-address=0.0.0.0 --http-port=8080  --docroot=. 
popd
fi

exit