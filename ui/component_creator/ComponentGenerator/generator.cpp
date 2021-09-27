#include "generator.h"
#include "installer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <regex>

#include "object/generator.h"
#include "protocol/generator.h"
#include "transport/generator.h"
#include "custom/generator.h"

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
int generate_component_code(const gengetopt_args_info *ai)
{
    std::string name = ai->name_arg;

    if (!std::regex_match(name, std::regex("^[A-Za-z][A-Za-z0-9_]*$")))
    {
        std::cout << "Error: project name is not a correct C++ identifier\n";
        return 1;
    }

    std::string library_name = ai->library_name_arg;

    if (!std::regex_match(library_name, std::regex("^[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*$")))
    {
        std::cout << "Error: library name is not of OSP format\n";
        return 1;
    }

    if (ai->output_directory_given)
    {
        if (create_directories(ai->output_directory_arg))
            return 1;

        std::error_code ec;
        std::filesystem::current_path(ai->output_directory_arg, ec);
        if (ec)
        {
            std::cout << "std::filesystem::current_path(\"" << ai->output_directory_arg
                <<"\"): error code = " << ec << "\n";
            return 1;
        }

        std::cout << "Changed directory to \"" << ai->output_directory_arg << "\"\n";
    }

    std::map<std::string, std::string> dict;
    {
        dict["name"] = name;

        std::transform(name.begin(), name.end(), name.begin(),
                [](unsigned char c){ return std::tolower(c); });
        dict["name-lowercase"] = name;

        std::transform(name.begin(), name.end(), name.begin(),
                [](unsigned char c){ return std::toupper(c); });
        dict["name-uppercase"] = name;

        if (!ai->library_name_given)
        {
            switch (ai->type_arg)
            {
                case type_arg_protocol:
                    library_name = "ds.host.protocols.protocol.test";
                    break;

                case type_arg_transport:
                    library_name = "ds.host.transport.provider.test";
                    break;
            }
        }
        dict["library-name"] = library_name;

        dict["include-directory"] = ai->include_directory_arg;
    }

    switch (ai->type_arg)
    {
        case type_arg_object:
            return generate_object_component_code(ai, dict);

        case type_arg_protocol:
            return generate_protocol_component_code(ai, dict);

        case type_arg_transport:
            return generate_transport_component_code(ai, dict);

        case type_arg_custom:
            return generate_custom_component_code(ai, dict);

        default:
            std::cout << "Unknown component type " << ai->type_orig << "\n";
            return 1;
    }
}

