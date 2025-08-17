cmake_minimum_required(VERSION 3.30)
set(CMAKE_BUILD_TYPE Debug)
project (wt.extra)

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "WT_CONFIG_H: " ${WT_CONFIG_H})
message(STATUS "BOOST_INCLUDE_DIR: " ${BOOST_INCLUDE_DIR})
message(STATUS "BOOST_LIB_DIRS: " ${BOOST_LIB_DIRS})

set (CMAKE_CXX_STANDARD 14)

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${WT_INCLUDE})
include_directories(${WT_CONFIG_H})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/ext/wt-4.12.0/src)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

#//////////////////////////
# source files
#//////////////////////////

set(src ${src})
set(src ${src} src/map.cc)
add_executable(map ${src})

#//////////////////////////
# set map as startup project for Visual Studio
#//////////////////////////

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT map)
  set_property(TARGET map PROPERTY 
    VS_DEBUGGER_COMMAND_ARGUMENTS "--http-address=0.0.0.0 --http-port=8080  --docroot=. ")
  set_property(TARGET map PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
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
  message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
endif()

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
  set(lib_dep ${lib_dep} /Users/pvn/wt_install/lib/libwt.dylib)
  set(lib_dep ${lib_dep} /Users/pvn/wt_install/lib/libwthttp.dylib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.a)
endif()

if (LINUX)
  set(lib_dep ${lib_dep} /home/pvn/wt_install/lib/libwt.so)
  set(lib_dep ${lib_dep} /home/pvn/wt_install/lib/libwthttp.so)
  set(BOOST_LIB_DIRS /home/pvn/wt.extra/build/boost_1_88_0/lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.so)
endif()

#//////////////////////////
#  link
#//////////////////////////

message(STATUS "lib_dep: " ${lib_dep})
target_link_libraries (map ${lib_dep})
