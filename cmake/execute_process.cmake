execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ui/component_creator/interfaces bin/interfaces
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ui/resources/xml bin/xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ui/resources/deploy bin/deploy
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/languages bin/languages
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ui/resources/xml bin/components
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

#if (WIN32)

#add_custom_command(TARGET ${IDF_PROJECT_EXECUTABLE}
#                   POST_BUILD
#
#                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Samples.zip
#                   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
#
#                   COMMENT "Copying esp32 binary output to SomeFolder"
#                   )

# create dir bndl