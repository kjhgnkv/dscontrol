#include "generator.h"
#include "installer.h"

#include <filesystem>
#include <iostream>

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
int generate_protocol_component_code(const gengetopt_args_info *ai, std::map<std::string, std::string>& dict)
{
    std::string includeDir = "include/DSHost/Protocol/";
    includeDir += dict["name"];
    includeDir += "/";

    std::string includeImplDir = includeDir + "Impl/";
    std::string srcDir = "src/";

    if (create_directories(includeDir) || create_directories(includeImplDir) || create_directories(srcDir))
        return 1;
    
    if (
            install_file("macchina.cmake",
//#include "protocol/macchina.cmake.string"
             getFileContent("://templates/protocol/macchina.cmake")
                ) ||
            install_file("build.cmake",
            getFileContent("://templates/protocol/build.cmake")
//#include "protocol/build.cmake.string"
                )
       )
        return 1;

    if (
            install_file("CMakeLists.txt",
            getFileContent("://templates/protocol/CMakeLists.txt")
//#include "protocol/CMakeLists.txt.string"
                , dict) ||
            install_file("Bundle.bndlspec",
            getFileContent("://templates/protocol/Bundle.bndlspec")
//#include "protocol/Bundle.bndlspec.string"
                , dict) ||
            install_file("Activator.cpp",
            getFileContent("://templates/protocol/Activator.cpp")
//#include "protocol/Activator.cpp.string"
                , dict)
       )
        return 1;

    if (
            install_file(includeDir + "Defines.h",
            getFileContent("://templates/protocol/include/Defines.h")
//#include "protocol/include/Defines.h.string"
                , dict) ||
            install_file(includeDir + "Message.h",
            getFileContent("://templates/protocol/include/Message.h")
//#include "protocol/include/Message.h.string"
                , dict) ||
            install_file(includeDir + "Protocol.h",
            getFileContent("://templates/protocol/include/Protocol.h")
//#include "protocol/include/Protocol.h.string"
                , dict) ||
            install_file(includeDir + "ProtocolFactory.h",
            getFileContent("://templates/protocol/include/ProtocolFactory.h")
//#include "protocol/include/ProtocolFactory.h.string"
                , dict) ||
            install_file(includeDir + "RawConverter.h",
            getFileContent("://templates/protocol/include/RawConverter.h")
//#include "protocol/include/RawConverter.h.string"
                , dict) ||
            install_file(includeDir + "StandardInternalConverter.h",
            getFileContent("://templates/protocol/include/StandardInternalConverter.h")
//#include "protocol/include/StandardInternalConverter.h.string"
                , dict) ||
            install_file(includeDir + "MessageAuxDataModifier.h",
            getFileContent("://templates/protocol/include/MessageAuxDataModifier.h")
//#include "protocol/include/MessageAuxDataModifier.h.string"
                , dict)
            )
            return 1;

    if (
            install_file(includeImplDir + "Message.h",
            getFileContent("://templates/protocol/include/Impl/Message.h")
//#include "protocol/include/Impl/Message.h.string"
                , dict) ||
            install_file(includeImplDir + "Protocol.h",
            getFileContent("://templates/protocol/include/Impl/Protocol.h")
//#include "protocol/include/Impl/Protocol.h.string"
                , dict) ||
            install_file(includeImplDir + "ProtocolFactory.h",
            getFileContent("://templates/protocol/include/Impl/ProtocolFactory.h")
//#include "protocol/include/Impl/ProtocolFactory.h.string"
                , dict) ||
            install_file(includeImplDir + "RawConverter.h",
            getFileContent("://templates/protocol/include/Impl/RawConverter.h")
//#include "protocol/include/Impl/RawConverter.h.string"
                , dict) ||
            install_file(includeImplDir + "StandardInternalConverter.h",
            getFileContent("://templates/protocol/include/Impl/StandardInternalConverter.h")
//#include "protocol/include/Impl/StandardInternalConverter.h.string"
                , dict) ||
            install_file(includeImplDir + "MessageAuxDataModifier.h",
            getFileContent("://templates/protocol/include/Impl/MessageAuxDataModifier.h")
//#include "protocol/include/Impl/MessageAuxDataModifier.h.string"
                , dict)
       )
        return 1;

    if (
            install_file(srcDir + "RawConverter.cpp",
            getFileContent("://templates/protocol/src/RawConverter.cpp")
//#include "protocol/src/RawConverter.cpp.string"
                , dict) ||
            install_file(srcDir + "RawConverterFeatures.cpp",
            getFileContent("://templates/protocol/src/RawConverterFeatures.cpp")
//#include "protocol/src/RawConverterFeatures.cpp.string"
                , dict) ||
            install_file(srcDir + "StandardInternalConverter.cpp",
            getFileContent("://templates/protocol/src/src/StandardInternalConverter.cpp")
//#include "protocol/src/StandardInternalConverter.cpp.string"
                , dict) ||
            install_file(srcDir + "MessageAuxDataModifier.cpp",
            getFileContent("://templates/protocol/src/MessageAuxDataModifier.cpp")
//#include "protocol/src/MessageAuxDataModifier.cpp.string"
                , dict)
       )
        return 1;
        
    return 0;
}

