cmake_minimum_required(VERSION 3.20)
set(CMAKE_BUILD_TYPE Debug)
project (wt.extra)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "BOOST_INCLUDE_DIR: " ${BOOST_INCLUDE_DIR})
message(STATUS "BOOST_LIB_DIRS: " ${BOOST_LIB_DIRS})

set(CMAKE_CXX_STANDARD 14)

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${WT_INCLUDE})
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

#//////////////////////////
# build Wt on Windows (debug sources)
#//////////////////////////

if (MSVC)
  set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build/boost_1_88_0")
  set(Boost_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/boost_1_88_0/include")
  set(DBoost_LIBRARY_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/build/boost_1_88_0/lib")
  set(BUILD_EXAMPLES OFF)
  set(BUILD_TESTS OFF)
  set(SHARED_LIBS OFF)
  add_subdirectory(ext/wt-4.12.0)
  set_target_properties(wt PROPERTIES FOLDER "ext")
  set_target_properties(wthttp PROPERTIES FOLDER "ext")
  set_target_properties(doc PROPERTIES FOLDER "ext")
  set_target_properties(wtdbo PROPERTIES FOLDER "ext")
  set_target_properties(wtdbosqlite3 PROPERTIES FOLDER "ext")
  set_target_properties(wttest PROPERTIES FOLDER "ext")
  set_target_properties(wtdbomssqlserver PROPERTIES FOLDER "ext")
  set_target_properties(wtisapi PROPERTIES FOLDER "ext")
endif()

#//////////////////////////
# source files
#//////////////////////////

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/mapbox")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ext/wt-4.12.0/src")
set(src ${src})
set(src ${src} src/mapbox/WMapbox.cc)
set(src ${src} src/mapbox/WMapbox.hh)
set(src ${src} src/mapbox.cc)
set(src ${src} src/requests.cc)
set(src ${src} src/parser.cc)
set(src ${src} src/parser.hh)
add_executable(mapbox ${src})
add_executable(leaflet src/leaflet.cc)
add_executable(splitter src/splitter.cc)
add_executable(parser src/parse.cc src/parser.cc src/parser.hh)

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT mapbox)
  set_property(TARGET mapbox PROPERTY 
    VS_DEBUGGER_COMMAND_ARGUMENTS "--http-address=0.0.0.0 --http-port=8080  --docroot=. -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson")
  set_property(TARGET mapbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
# Linux/Mac
#//////////////////////////

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-deprecated-declarations")
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-deprecated-copy-dtor")
endif()

message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

#//////////////////////////
# LINUX
# order of the link libraries matters; pthread dl
#//////////////////////////

if(LINUX)
  set(lib_dep ${lib_dep} pthread dl)
  set(lib_dep ${lib_dep} stdc++fs) 
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

if (MSVC)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/build/wt-4.12.0/src/Debug/wtd.lib)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/build/wt-4.12.0/src/http/Debug/wthttpd.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_chrono-vc143-mt-gd-x64-1_88.lib)
endif()

if(APPLE)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwt.dylib)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwthttp.dylib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem-clang-darwin17-mt-x64-1_88.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread-clang-darwin17-mt-x64-1_88.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options-clang-darwin17-mt-x64-1_88.a)
endif()

if (LINUX)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwt.so)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwthttp.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem-gcc13-mt-x64-1_88.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread-gcc13-mt-x64-1_88.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options-gcc13-mt-x64-1_88.a)
endif()

#//////////////////////////
#  link
#//////////////////////////

message(STATUS "lib_dep: " ${lib_dep})
target_link_libraries (mapbox ${lib_dep})
target_link_libraries (leaflet ${lib_dep})

#//////////////////////////
# data files 
#//////////////////////////

message(STATUS "Copying wt_config.xml to: ${CMAKE_SOURCE_DIR}/install/wt/etc/wt)")
file(COPY "${CMAKE_SOURCE_DIR}/resources/wt_config_leaflet.xml" DESTINATION ${CMAKE_SOURCE_DIR}/install/wt/etc/wt)

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
set(DATA_FILES
  "resources/dc_311-2016.csv.s0311.csv"
  "resources/ward-2012.geojson"
  "resources/311_city_service_requests_2024_part1.csv"
  "resources/311_city_service_requests_2024_part2.csv"
)

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
foreach(DATA_FILE ${DATA_FILES})
  file(COPY "${CMAKE_SOURCE_DIR}/${DATA_FILE}" DESTINATION ${CMAKE_BINARY_DIR})
endforeach()
