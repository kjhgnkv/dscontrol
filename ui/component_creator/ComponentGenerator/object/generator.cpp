#include "generator.h"
#include "installer.h"

#include <filesystem>
#include <iostream>
#include <regex>

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
int generate_object_component_code(const gengetopt_args_info *ai, std::map<std::string, std::string>& dict)
{
    {
        std::string nm_begin, nm_end, nm_prefix;

        std::string name_space = ai->namespace_arg;
        if (!name_space.empty())
        {
            if (!std::regex_match(name_space, std::regex("^[A-Za-z][A-Za-z0-9_]*(::[A-Za-z][A-Za-z0-9_]*)*$")))
            {
                std::cout << "Error: code namespace is invalid\n";
                return 1;
            }

            size_t pos = 0, next_pos;
            std::string sub;
            while (pos != std::string::npos)
            {
                next_pos = name_space.find("::", pos);

                if (next_pos == std::string::npos)
                {
                    sub = name_space.substr(pos);
                    pos = next_pos;
                }
                else
                {
                    sub = name_space.substr(pos, next_pos - pos);
                    pos = next_pos + 2;
                }

                if (nm_begin.empty())
                    nm_begin = "namespace ";
                else
                    nm_begin += "\nnamespace ";
                nm_begin += sub;
                nm_begin += " {";

                nm_end += "} ";
            }

            nm_end += "// namespace ";
            nm_end += name_space;

            nm_prefix = name_space + "::";
        }

        dict["namespace-begin"] = nm_begin;
        dict["namespace-end"] = nm_end;
        dict["namespace-prefix"] = nm_prefix;
    }

    std::string includeDir = "include/";
    std::string srcDir = "src/";

    if (create_directories(includeDir) || create_directories(srcDir))
        return 1;

    if (ai->qt_gui_given)
    {
        if (
                install_file("macchina.cmake",
//#include "object_gui/macchina.cmake.string"
            getFileContent("://templates/object_gui/macchina.cmake")
                    ) ||
                install_file("build.cmake",
//#include "object_gui/build.cmake.string"
            getFileContent("://templates/object_gui/build.cmake")
                    )
           )
            return 1;

        if (
                install_file("CMakeLists.txt",
//#include "object_gui/CMakeLists.txt.string"
            getFileContent("://templates/object_gui/CMakeLists.txt")
                    , dict) ||
                install_file("Bundle.bndlspec",
//#include "object_gui/Bundle.bndlspec.string"
            getFileContent("://templates/object_gui/Bundle.bndlspec")
                    , dict)
           )
            return 1;

        if (
                install_file(includeDir + "BundleObject.h",
//#include "object_gui/include/BundleObject.h.string"
            getFileContent("://templates/object_gui/include/BundleObject.h")
                    , dict) ||
                install_file(includeDir + "QtWidget.h",
//#include "object_gui/include/QtWidget.h.string"
            getFileContent("://templates/object_gui/include/QtWidget.h")
                    , dict) ||
                install_file(includeDir + "QtDesignerPlugin.h",
//#include "object_gui/include/QtDesignerPlugin.h.string"
            getFileContent("://templates/object_gui/include/QtDesignerPlugin.h")
                    , dict)
           )
            return 1;

        if (
                install_file(srcDir + "BundleObject.cpp",
//#include "object_gui/src/BundleObject.cpp.string"
            getFileContent("://templates/object_gui/src/BundleObject.cpp")
                    , dict) ||
                install_file(srcDir + "QtWidget.cpp",
//#include "object_gui/src/QtWidget.cpp.string"
            getFileContent("://templates/object_gui/src/QtWidget.cpp")
                    , dict) ||
                install_file(srcDir + "QtDesignerPlugin.cpp",
//#include "object_gui/src/QtDesignerPlugin.cpp.string"
            getFileContent("://templates/object_gui/src/QtDesignerPlugin.cpp")
                    , dict)
           )
            return 1;
    }
    else
    {
        if (
                install_file("macchina.cmake",
//#include "object_cli/macchina.cmake.string"
            getFileContent("://templates/object_cli/macchina.cmake")
                    ) ||
                install_file("build.cmake",
//#include "object_cli/build.cmake.string"
            getFileContent("://templates/object_cli/build.cmake")
                    )
           )
            return 1;

        if (
                install_file("CMakeLists.txt",
//#include "object_cli/CMakeLists.txt.string"
            getFileContent("://templates/object_cli/CMakeLists.txt")
                    , dict) ||
                install_file("Bundle.bndlspec",
//#include "object_cli/Bundle.bndlspec.string"
            getFileContent("://templates/object_cli/Bundle.bndlspec")
                    , dict)
           )
            return 1;

        if (
                install_file(includeDir + "BundleObject.h",
//#include "object_cli/include/BundleObject.h.string"
            getFileContent("://templates/object_cli/include/BundleObject.h")
                    , dict)
           )
            return 1;

        if (
                install_file(srcDir + "BundleObject.cpp",
//#include "object_cli/src/BundleObject.cpp.string"
            getFileContent("://templates/object_cli/src/BundleObject.cpp")
                    , dict)
           )
            return 1;
    }
    
    return 0;
}

