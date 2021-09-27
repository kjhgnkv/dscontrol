# Required arguments
# QTC_BUILD - directory, where QtCreator was built from sources
# QTC_SOURCE - directory, where QtCreator source files are
# DS_LIB - path to libDS.so or libDS.dll file
# QTC_USER_DIR - directory, where will be your plugin

cmake_minimum_required(VERSION 3.12)

project(DSplugin)

# define command for make execute
if(UNIX)
    set(MAKE "make")
else()
    set(MAKE "mingw32-make.exe")
endif()

# temporary build directory for qmake
set(QMAKE_BUILD_DIR "../qmake_build")

# remove and create temporary build directory for qmake
execute_process(COMMAND rm -r ${QMAKE_BUILD_DIR})
execute_process(COMMAND mkdir ${QMAKE_BUILD_DIR})

# execute qmake with required arguments
execute_process(
    COMMAND qmake .. QTC_BUILD=${QTC_BUILD} QTC_SOURCE=${QTC_SOURCE} DS_LIB=${DS_LIB}
    WORKING_DIRECTORY ${QMAKE_BUILD_DIR}
)

# compile plugin
execute_process(
    COMMAND ${MAKE}
    WORKING_DIRECTORY ${QMAKE_BUILD_DIR}
)

# remove temporary build directory for qmake after compiling
execute_process(COMMAND rm -r ${QMAKE_BUILD_DIR})

# copy DSPlatform lib to directory, where plugin has created
execute_process(COMMAND cp ${DS_LIB} ${QTC_USER_DIR})
