# cmake for component_creator
# WARN for some reason on windows custom target copy_needed_components not run
# when we compile project. So you have to do it manually

project(component_creator)

include(qt)
include(macchina)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

qt5_add_resources(SOURCES xml_schema_res.qrc)
qt5_add_resources(SOURCES ComponentGenerator/templates.qrc)
include(include.cmake)


add_library(${PROJECT_NAME} ${DLLTYPE}
            ${COMP_CREATOR_INCLUDE}
            ${COMP_CREATOR_SRC}
            ${COMP_CREATOR_UI}
            ${SOURCES}
            )

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
						   ${Macchina_INCLUDE_DIRS}
                           ../../
						   ComponentGenerator
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})

#set(DS_OUT_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/ds_files)

#get_filename_component(DS_OUT_FILES_DIR ${DS_OUT_FILES_DIR} ABSOLUTE)

#configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#add_custom_target(copy_needed_files ALL
#  COMMAND ${CMAKE_COMMAND} -E copy_directory xmls ${DS_OUT_FILES_DIR}/xmls
#  COMMAND ${CMAKE_COMMAND} -E copy FindDS.cmake ${DS_OUT_FILES_DIR}/FindDS.cmake
#COMMAND ${CMAKE_COMMAND} -E copy_directory ../../_FromSun/DS ${DS_OUT_FILES_DIR}/DS
#  DEPENDS ${PROJECT_NAME}
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMENT Copy needed files in system directory
#  )
