cmake_minimum_required(VERSION 3.13)
project(ModuleTestExampleDSControl)

set(CMAKE_CXX_STANDARD 98)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wno-sign-compare)
endif(CMAKE_COMPILER_IS_GNUCC)

include(build.cmake)

if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

include(../../../DSHost/macchina.cmake)

set(Macchina_LIBRARIES_RELEASE PocoFoundation PocoUtil PocoOSP)
set(Macchina_LIBRARIES_DEBUG PocoFoundationd PocoUtild PocoOSPd)

add_library(ModuleTestExampleDSControl MODULE)
set_target_properties(ModuleTestExampleDSControl PROPERTIES 
    OUTPUT_NAME ds.host.samples.test_example_dscontrol
    PREFIX "")
target_sources(ModuleTestExampleDSControl PRIVATE
    TestExampleDSControlObject.cpp
    Logger.cpp
    Controller.cpp
    Handler/CommandHandler.cpp
    Handler/MessageHandler.cpp
    Commands/ReceiveComponentCommand.cpp
    Commands/ReceiveApplicationTemplateCommand.cpp
    Commands/DeployCommand.cpp
    Commands/GetDSControlDataCommand.cpp
    Commands/SetDSControlDataCommand.cpp
    Commands/StopCommand.cpp
    Commands/UpdateCommand.cpp
    )
target_include_directories(ModuleTestExampleDSControl PRIVATE 
    ..
    ../../../ServerComponents/components/transport/BroadcasterWithMQTT/include
    ../../../businessComponents/DSServer
    ../../../DSHost/include
    "${Macchina_INCLUDE_DIRS}"
    ${CMAKE_SOURCE_DIR}/../../../libraries/DSUtils
    )
target_link_directories(ModuleTestExampleDSControl PRIVATE 
    "${Macchina_LIBRARY_DIRS}"
    )
target_link_libraries(ModuleTestExampleDSControl PRIVATE 
    optimized "${Macchina_LIBRARIES_RELEASE}"
    debug "${Macchina_LIBRARIES_DEBUG}"
    )
target_compile_options(ModuleTestExampleDSControl PRIVATE 
    "-std=c++03"
    )
# target_compile_definitions(ModuleTestExampleDSControl PRIVATE -DUNICODE)

