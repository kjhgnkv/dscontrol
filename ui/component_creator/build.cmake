# build.cmake

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
include(CTest)

message(STATUS "buid type    " ${CMAKE_BUILD_TYPE})
message(STATUS "Project      " ${PROJECT_NAME})
message(STATUS "c compiler   " ${CMAKE_C_COMPILER})
message(STATUS "cxx compiler " ${CMAKE_CXX_COMPILER})
message(STATUS "build tests  " ${BUILD_TESTING})
message(STATUS "build shared " ${BUILD_SHARED_LIBS})

#if (${CMAKE_BUILD_TYPE} STREQUAL Debug AND MSVC)
#    set(BUILD_SUFFIX "d")
#message("BUILD_SUFFIX:" ${BUILD_SUFFIX})
#endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug build.")
    set(BUILD_SUFFIX "d")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    message("Release build.")
else ()
    message("Some other build type.")
endif ()

if (UNIX)
    set(DLLTYPE SHARED)
    set(EXETYPE "")

    message("Build type UNIX: SHARED")
elseif (WIN32)
    set(DLLTYPE STATIC)
    set(EXETYPE ".exe")

    message("Build type WIN32: STATIC")
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O0")
if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wnon-virtual-dtor -pedantic")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

option(leak_check "set leak_check" 0)
option(thread_check "set thread_check" 0)
option(profiling "set profiling" 0)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug AND leak_check)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug AND thread_check)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug AND profiling)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -pg")
    # set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -pg")
endif ()

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wnon-virtual-dtor -pedantic")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /experimental:external /external:anglebrackets /external:W0")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /experimental:external /external:anglebrackets /external:W0")
    endif()
endif ()
