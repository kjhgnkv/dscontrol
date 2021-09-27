# create_installer.cmake

if (WIN32)
    set(BINARY_EXTENTION ".exe")
endif ()

set(BINARYCREATOR_PATH ${CMAKE_PREFIX_PATH}/../../Tools/QtInstallerFramework/3.2/bin/binarycreator${BINARY_EXTENTION})

if (EXISTS ${BINARYCREATOR_PATH} AND ${CMAKE_BUILD_TYPE} STREQUAL Release AND ((WIN32 AND "${CMAKE_CXX_COMPILER}" MATCHES "mingw") OR UNIX))
    set(INSTALLER_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/installator)
    set(INSTALLER_PACKAGES_DIR ${INSTALLER_DIR}/packages)
    set(INSTALLER_PACKAGE_DATA_DIR ${INSTALLER_PACKAGES_DIR}/DSControl/data/DSControl)
    set(INSTALLER_NAME "${PROJECT_NAME}_installer")

    add_custom_target(copy_files_for_installator ALL
                      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/installer ${INSTALLER_DIR}
                      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/xml ${INSTALLER_PACKAGE_DATA_DIR}/xml
                      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/languages ${INSTALLER_PACKAGE_DATA_DIR}/languages
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}${BINARY_EXTENTION} ${INSTALLER_PACKAGE_DATA_DIR}
                      DEPENDS ${PROJECT_NAME} copy_all_xml_files copy_all_languages_files
                      COMMENT "copy files for installer"
                      )

    if (WIN32)
        set(LIB_DS_NAME libDS.dll)
        add_custom_target(copy_libds_for_installator ALL
                          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_DS_NAME} ${INSTALLER_PACKAGE_DATA_DIR}
                          DEPENDS ${PROJECT_NAME} copy_files_for_installator
                          COMMENT "copy libDS.dll for installer"
                          )

        message(STATUS "Installing system-libraries: MinGW DLLs.")
        get_filename_component(MINGW_PATH ${CMAKE_CXX_COMPILER} PATH)
        if (MXE_BUILD)
            message(STATUS "Installing for MXE!")
            set(LIB_PATH ${MINGW_PATH}/../../x86_64-w64-mingw32.shared/bin)
        else ()
            message(STATUS "Installing for MinGW!")
            set(LIB_PATH ${MINGW_PATH})
        endif ()
        set(MINGW_RUNTIME_LIBS ${LIB_PATH}/libgcc_s_seh-1.dll ${LIB_PATH}/libstdc++-6.dll ${LIB_PATH}/libwinpthread-1.dll)

        add_custom_target(copy_compiler_libs_for_installer ALL
                          COMMAND ${CMAKE_COMMAND} -E copy ${MINGW_RUNTIME_LIBS} ${INSTALLER_PACKAGE_DATA_DIR}
                          DEPENDS ${PROJECT_NAME} copy_libds_for_installator
                          COMMENT "copy libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll for installer"
                          )

        add_custom_target(copy_dependencies_for_lib_ds ALL
                          COMMAND windeployqt.exe --verbose 0 --release ${INSTALLER_PACKAGE_DATA_DIR}/${LIB_DS_NAME}
                          DEPENDS ${PROJECT_NAME} copy_compiler_libs_for_installer
                          COMMENT "find and copy dependencies for libDS.dll"
                          )

        add_custom_target(copy_dependencies_for_ds_control ALL
                          COMMAND windeployqt.exe --verbose 0 --release ${INSTALLER_PACKAGE_DATA_DIR}/${PROJECT_NAME}${BINARY_EXTENTION}
                          DEPENDS ${PROJECT_NAME} copy_dependencies_for_lib_ds
                          COMMENT "find and copy dependencies for DS_Control"
                          )
    elseif (UNIX)
        message("Linux does not support dependency loading")
        add_custom_target(copy_dependencies_for_ds_control ALL
                          DEPENDS ${PROJECT_NAME} copy_files_for_installator
                          COMMENT "fake target"
                          )
    endif ()

    add_custom_target(create_installator ALL
                      COMMAND ${BINARYCREATOR_PATH} --offline-only -c ${INSTALLER_DIR}/config/config.xml -p ${INSTALLER_PACKAGES_DIR} ${INSTALLER_NAME}
                      COMMAND ${CMAKE_COMMAND} -E copy ${INSTALLER_NAME}${BINARY_EXTENTION} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                      COMMAND ${CMAKE_COMMAND} -E remove ${INSTALLER_NAME}${BINARY_EXTENTION}
                      COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALLER_DIR}
                      DEPENDS ${PROJECT_NAME} copy_dependencies_for_ds_control
                      COMMENT "create installator"
                      )
elseif (NOT EXISTS ${BINARYCREATOR_PATH})
    message("QtInstallFramework not found")
elseif (NOT ${CMAKE_BUILD_TYPE} STREQUAL Release)
    message("Build type not Release")
elseif (NOT (WIN32 AND "${CMAKE_CXX_COMPILER}" MATCHES "mingw"))
    message("compiler isn't mingw")
endif ()
