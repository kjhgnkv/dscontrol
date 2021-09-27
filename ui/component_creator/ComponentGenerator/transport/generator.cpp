#include "generator.h"
#include "installer.h"

#include <filesystem>
#include <iostream>
#include "global.h"

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
int generate_transport_component_code(const gengetopt_args_info *ai, std::map<std::string, std::string>& dict)
{
    std::string includeDir = "include/DSHost/Transport/";
    includeDir += dict["name"];
    includeDir += "/";

    std::string includeImplDir = includeDir + "Impl/";
    std::string srcDir = "src/";

    if (create_directories(includeDir) || create_directories(includeImplDir) || create_directories(srcDir))
        return 1;

    if (
            install_file("macchina.cmake",
            getFileContent("://templates/transport/macchina.cmake")
//#include "transport/macchina.cmake.string"
                ) ||
            install_file("build.cmake",
            getFileContent("://templates/transport/build.cmake")
//#include "transport/build.cmake.string"
                )
       )
        return 1;

    if (
            install_file("CMakeLists.txt",
            getFileContent("://templates/transport/CMakeLists.txt")
//#include "transport/CMakeLists.txt.string"
                , dict) ||
            install_file("Bundle.bndlspec",
            getFileContent("://templates/transport/Bundle.bndlspec")
//#include "transport/Bundle.bndlspec.string"
                , dict) ||
            install_file("Activator.cpp",
            getFileContent("://templates/transport/Activator.cpp")
//#include "transport/Activator.cpp.string"
                , dict)
       )
        return 1;

    if (
            install_file(includeDir + "Defines.h",
            getFileContent("://templates/transport/include/Defines.h")
//#include "transport/include/Defines.h.string"
                , dict) ||
            install_file(includeDir + "Channel.h",
            getFileContent("://templates/transport/include/Channel.h")
//#include "transport/include/Channel.h.string"
                , dict) ||
            install_file(includeDir + "ChannelFactory.h",
            getFileContent("://templates/transport/include/ChannelFactory.h")
//#include "transport/include/ChannelFactory.h.string"
                , dict)
            )
            return 1;

    if (
            install_file(includeImplDir + "Channel.h",
            getFileContent("://templates/transport/include/Impl/Channel.h")
//#include "transport/include/Impl/Channel.h.string"
                , dict) ||
            install_file(includeImplDir + "ChannelFactory.h",
            getFileContent("://templates/transport/include/Impl/ChannelFactory.h")
//#include "transport/include/Impl/ChannelFactory.h.string"
                , dict)
       )
        return 1;

    if (
            install_file(srcDir + "Channel.cpp",
            getFileContent("://templates/transport/src/Channel.cpp")
//#include "transport/src/Channel.cpp.string"
                , dict)
       )
        return 1;

    return 0;
}

