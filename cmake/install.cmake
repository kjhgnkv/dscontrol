# NOTE for linux you need install `libxcb-xinerama0` - it neded for qt plugins
# and neded set LD_LIBRARY_PATH to directory, where you put executable

include(InstallRequiredSystemLibraries)

# for MXE
if (${CMAKE_BUILD_TYPE} STREQUAL Release AND "${CMAKE_CXX_COMPILER}" MATCHES "mingw")
    message(STATUS "Installing system-libraries: MinGW DLLs.")
    get_filename_component(Mingw_Path ${CMAKE_CXX_COMPILER} PATH)
    if (MXE_BUILD)
        message(STATUS "Installing for MXE!")
        set(Lib_Path ${Mingw_Path}/../../x86_64-w64-mingw32.shared/bin)
    else ()
        message(STATUS "Installing for MinGW!")
        set(Lib_Path ${Mingw_Path})
    endif ()
    set(CMAKE_INSTALL_MINGW_RUNTIME_LIBS ${Lib_Path}/libgcc_s_seh-1.dll ${Lib_Path}/libstdc++-6.dll ${Lib_Path}/libwinpthread-1.dll)
endif ()

#find_package(Qt5 COMPONENTS Widgets Core Network Gui Sql Xml PrintSupport Svg REQUIRED)

set(QT_MAIN_PATH "${Qt5_DIR}/../../..")

if (WIN32)
    find_package(Qt5 COMPONENTS WinExtras REQUIRED)
elseif (UNIX)
    find_package(Qt5 COMPONENTS DBus REQUIRED)
    # Qt haven't cmake file for XcbQpa, so it couldn't be find as
    # module. Here I manually search neded libraries.
    find_library(XcbQpa NAMES "libQt5XcbQpa.so.5" PATHS "${QT_MAIN_PATH}/lib")
    find_library(XcbQpa_SONAME NAMES "libQt5XcbQpa.so.${Qt5_VERSION}" PATHS "${QT_MAIN_PATH}/lib")
endif ()

install(
        TARGETS ${PROJECT_NAME} CONFIGURATIONS Release DESTINATION bin
)

if (CMAKE_INSTALL_MINGW_RUNTIME_LIBS)
    install(PROGRAMS ${CMAKE_INSTALL_MINGW_RUNTIME_LIBS} DESTINATION bin COMPONENT System)
endif (CMAKE_INSTALL_MINGW_RUNTIME_LIBS)

install(
        FILES
        $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE:Qt5::Core> $<TARGET_FILE:Qt5::Network>
        $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE:Qt5::PrintSupport> $<TARGET_FILE:Qt5::Sql>
        $<TARGET_FILE:Qt5::Xml> $<TARGET_FILE:Qt5::XmlPatterns> $<TARGET_FILE:Qt5::Svg>
        CONFIGURATIONS Release
        DESTINATION bin
)

install(
        FILES
        $<TARGET_FILE:DS> $<TARGET_FILE:nodeeditor>
        CONFIGURATIONS Release
        DESTINATION bin
)

if (UNIX)
    install(
            FILES
            $<TARGET_SONAME_FILE:Qt5::Widgets> $<TARGET_SONAME_FILE:Qt5::Core> $<TARGET_SONAME_FILE:Qt5::Network>
            $<TARGET_SONAME_FILE:Qt5::Gui> $<TARGET_SONAME_FILE:Qt5::PrintSupport> $<TARGET_SONAME_FILE:Qt5::Sql>
            $<TARGET_SONAME_FILE:Qt5::Xml> $<TARGET_SONAME_FILE:Qt5::XmlPatterns> $<TARGET_SONAME_FILE:Qt5::Svg>
            CONFIGURATIONS Release
            DESTINATION bin
    )
    install(
            FILES
            $<TARGET_FILE:Qt5::DBus>
            $<TARGET_SONAME_FILE:Qt5::DBus>
            ${XcbQpa}
            ${XcbQpa_SONAME}
            CONFIGURATIONS Release
            DESTINATION bin
    )
elseif (WIN32)
    install(
            FILES $<TARGET_FILE:Qt5::WinExtras> CONFIGURATIONS Release DESTINATION bin
    )
endif ()

set(QT_PLATFORMS_PATH "${QT_MAIN_PATH}/plugins/platforms")
set(QT_IMAGEFORMATS_PATH "${QT_MAIN_PATH}/plugins/imageformats")
install(
        DIRECTORY
        ${QT_PLATFORMS_PATH}
        ${QT_IMAGEFORMATS_PATH} DESTINATION bin
)
