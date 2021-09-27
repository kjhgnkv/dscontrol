set(name ModuleTestRPC)

set(Macchina_LIBRARIES_RELEASE PocoFoundation PocoUtil PocoOSP)
set(Macchina_LIBRARIES_DEBUG PocoFoundationd PocoUtild PocoOSPd)

add_library(ModuleTestRPC MODULE)
set_target_properties(ModuleTestRPC PROPERTIES 
    OUTPUT_NAME ds.host.samples.rpc_test
    PREFIX "")
target_sources(ModuleTestRPC PRIVATE
    RPCTestBundleObject.cpp
    )
target_include_directories(ModuleTestRPC PRIVATE 
    .
    ../../DSHost/include
    ../../ServerComponents/components/extensions/RPC/include
    ../../libraries/DSUtils
    "${Macchina_INCLUDE_DIRS}"
    )
target_link_directories(ModuleTestRPC PRIVATE 
    "${Macchina_LIBRARY_DIRS}"
    )
target_link_libraries(ModuleTestRPC PRIVATE 
    optimized "${Macchina_LIBRARIES_RELEASE}"
    debug "${Macchina_LIBRARIES_DEBUG}"
    )
add_bundle(${name})
# target_compile_definitions(ModuleTestRPC PRIVATE -DUNICODE)

