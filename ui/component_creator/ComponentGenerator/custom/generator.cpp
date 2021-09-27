#include "generator.h"
#include "installer.h"

#include <filesystem>
#include <iostream>

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
int generate_custom_component_code(const gengetopt_args_info *ai, std::map<std::string, std::string>& dict)
{
    if (
            install_file("macchina.cmake",
            getFileContent("://templates/custom/macchina.cmake")
//#include "custom/macchina.cmake.string"
                ) ||
            install_file("build.cmake",
            getFileContent("://templates/custom/build.cmake")
//#include "custom/build.cmake.string"
                )
       )
        return 1;

    if (
            install_file("CMakeLists.txt",
            getFileContent("://templates/custom/CMakeLists.txt")
//#include "custom/CMakeLists.txt.string"
                , dict) ||
            install_file("Bundle.bndlspec",
            getFileContent("://templates/custom/Bundle.bndlspec")
//#include "custom/Bundle.bndlspec.string"
                , dict) ||
            install_file("Activator.cpp",
            getFileContent("://templates/custom/Activator.cpp")
//#include "custom/Activator.cpp.string"
                , dict)
       )
        return 1;

    return 0;
}

