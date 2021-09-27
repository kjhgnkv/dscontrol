set(name ModuleTestEcho)

set(Macchina_LIBRARIES_RELEASE PocoFoundation PocoUtil PocoOSP)
set(Macchina_LIBRARIES_DEBUG PocoFoundationd PocoUtild PocoOSPd)

add_library(${name} MODULE)
set_target_properties(${name} PROPERTIES 
    OUTPUT_NAME ds.host.samples.echo
    PREFIX "")
target_sources(${name} PRIVATE
    EchoBundleObject.cpp
    )
target_include_directories(${name} PRIVATE 
    .
    ../../../network/DSHost/include
    "${Macchina_INCLUDE_DIRS}"
	../../../network/libraries/DSUtils
    )
target_link_directories(${name} PRIVATE 
    "${Macchina_LIBRARY_DIRS}"
    )
target_link_libraries(${name} PRIVATE 
    ${Macchina_LIBRARIES}
    )
add_bundle(${name})
# target_compile_definitions(ModuleTestEcho PRIVATE -DUNICODE)

