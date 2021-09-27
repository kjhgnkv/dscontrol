#include "wizard.h"
#include <iostream>
#include <string>
#include <regex>

#include "object/wizard.h"
#include "protocol/wizard.h"
#include "transport/wizard.h"
#include "custom/wizard.h"

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
void execute_wizard(gengetopt_args_info *ai)
{
//     std::cout << "INTERACTIVE MODE:\n\n";
// 
    std::string value;
    int option;

    if (!ai->name_given)
    {
//         do
//         {
//             std::cout << "Enter project name (C++ indentifier, e.g. 'BetterClient'):\n> ";
//             std::getline(std::cin, value, '\n');
            value = ai->name_arg;

            if (!std::regex_match(value, std::regex("^[A-Za-z][A-Za-z0-9_]*$")))
            {
                std::cout << "Project name is not a correct C++ identifier, try again\n";
                //continue;
            }
            else
                ai->name_given = 1;
//                 break;
//         }
//         while (true);

//         ai->name_given = 1;
//         ai->name_arg = static_cast<char*>(malloc(value.length() + 1));
//         strncpy(ai->name_arg, value.c_str(), value.length() + 1);
    }

    if (!ai->library_name_given)
    {
//         do
//         {
//             std::cout << "Enter library name (in OSP format, e.g. 'ds.host.app.scheduler'):\n> ";
//             std::getline(std::cin, value, '\n');

            value = ai->library_name_arg;

            if (!std::regex_match(value, std::regex("^[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*$")))
            {
                std::cout << "Library name is not of OSP format, try again\n";
                //continue;
            }
            else
                ai->library_name_given = 1;
//                 break;
//         }
//         while (true);
// 
//         ai->library_name_given = 1;
//         ai->library_name_arg = static_cast<char*>(malloc(value.length() + 1));
//         strncpy(ai->library_name_arg, value.c_str(), value.length() + 1);
    }

    if (!ai->output_directory_given)
    {
//         do
//         {
//             std::cout << "Enter output code directory path (e.g. '/home/user/src/component'):\n> ";
//             std::getline(std::cin, value, '\n');
// 
//             break;
//         }
//         while (true);

        ai->output_directory_given = 1;
//         ai->output_directory_arg = static_cast<char*>(malloc(value.length() + 1));
//         strncpy(ai->output_directory_arg, value.c_str(), value.length() + 1);
    }

    if (!ai->include_directory_given)
    {
//         do
//         {
//             std::cout << "Enter path to directory with DSHost headers (e.g. '../../../DSHost/include'):\n> ";
//             std::getline(std::cin, value, '\n');
// 
//             break;
//         }
//         while (true);

        ai->include_directory_given = 1;
//         ai->include_directory_arg = static_cast<char*>(malloc(value.length() + 1));
//         strncpy(ai->include_directory_arg, value.c_str(), value.length() + 1);
    }

    if (!ai->type_given)
    {
//         do
//         {
//             std::cout << "Select component type:\n";
//             std::cout << type_arg_object    << ") object (business) component\n";
//             std::cout << type_arg_protocol  << ") protocol (server) component\n";
//             std::cout << type_arg_transport << ") transport (server) component\n";
//             std::cout << type_arg_custom    << ") custom component\n";
//             std::cout << "> ";
//             std::cin >> option;
//             std::cin.ignore(10, '\n');
// 
//             switch (option)
//             {
//                 case type_arg_object:
//                     ai->type_arg = type_arg_object;
//                     break;
// 
//                 case type_arg_protocol:
//                     ai->type_arg = type_arg_protocol;
//                     break;
// 
//                 case type_arg_transport:
//                     ai->type_arg = type_arg_transport;
//                     break;
// 
//                 case type_arg_custom:
//                     ai->type_arg = type_arg_custom;
//                     break;
// 
//                 default:
//                     std::cout << "Unknown component type, try again\n";
//                     continue;
//             }
// 
//             break;
//         }
//         while (true);

        ai->type_given = 1;
    }

    switch (ai->type_arg)
    {
        case type_arg_object:
            execute_object_component_wizard(ai);
            break;

        case type_arg_protocol:
            execute_protocol_component_wizard(ai);
            break;

        case type_arg_transport:
            execute_transport_component_wizard(ai);
            break;

        case type_arg_custom:
            execute_custom_component_wizard(ai);
            break;
    }

    //std::cout << "----------------------------------------\n\n";
}

