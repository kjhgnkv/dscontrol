# FindDS.cmake

include(FindPackageHandleStandardArgs)

# TODO this is directory with base interfaces, where is header files and libraries
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/DS")

find_path(DS_INCLUDE unicode.h PATH_SUFFIXES LibDS)
find_path(DSM_INCLUDE dll.cpp PATH_SUFFIXES LibDSM)
find_path(DSTL_INCLUDE ds.h PATH_SUFFIXES LibDSTL)

# needed for this libraries
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(DS_INCLUDE_DIR "${DS_INCLUDE};${DSM_INCLUDE};${DSTL_INCLUDE}")

if(UNIX)
  find_library(DS_LIBRARY libDS.a PATH_SUFFIXES libs)
  find_library(DSM_LIBRARY libDSM.a PATH_SUFFIXES libs)
  find_library(DSTL_LIBRARY libDSTL.a PATH_SUFFIXES libs)
elseif(WIN32)
  if (CMAKE_CXX_COMPILER MATCHES "mingw")
    find_library(DS_LIBRARY libDS.a PATH_SUFFIXES libs)
    find_library(DSM_LIBRARY libDSM.a PATH_SUFFIXES libs)
    find_library(DSTL_LIBRARY libDSTL.a PATH_SUFFIXES libs)
  elseif()
    find_library(DS_LIBRARY DS.lib PATH_SUFFIXES libs)
    find_library(DSM_LIBRARY DSM.lib PATH_SUFFIXES libs)
    find_library(DSTL_LIBRARY DSTL.lib PATH_SUFFIXES libs)
  endif()
else()
  message(FATAL_ERROR "unkhnow system")
endif()
set(DS_LIBRARIES "${DS_LIBRARY};${DSM_LIBRARY};${DSTL_LIBRARY};${CMAKE_THREAD_LIBS_INIT}")

find_package_handle_standard_args(DS FOUND_VAR DS_FOUND
  REQUIRED_VARS DS_LIBRARIES DS_INCLUDE_DIR)
