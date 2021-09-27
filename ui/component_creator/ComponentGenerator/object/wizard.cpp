#include "wizard.h"
#include <iostream>
#include <string>
#include <regex>

//TODO: GET RID OF COMMENTED CODE + CODE REFACTORING AFTER TESTING
// OR
//TODO: !!! REWRITE COMPONENT GENERATION TO OOP+QT STYLE?
void execute_object_component_wizard(gengetopt_args_info *ai)
{
    std::string value;

    if (!ai->namespace_given)
    {
//         do
//         {
//             std::cout << "Enter code namespace (empty string or indentifiers separated with ::, e.g. 'DS::app::server'):\n> ";
//             std::getline(std::cin, value, '\n');

            value = ai->namespace_arg;

            if (!value.empty() && !std::regex_match(value, std::regex("^[A-Za-z][A-Za-z0-9_]*(::[A-Za-z][A-Za-z0-9_]*)*$")))
            {
                std::cout << "Code namespace is invalid, try again\n";
                //continue;
            }
            else
                ai->namespace_given = 1;
//                break;
//         }
//         while (true);
// 
//         ai->namespace_given = 1;
//         ai->namespace_arg = static_cast<char*>(malloc(value.length() + 1));
//         strncpy(ai->namespace_arg, value.c_str(), value.length() + 1);
    }

//     if (!ai->qt_gui_given)
//     {
//         do
//         {
//             std::cout << "Enable Qt graphics support?\n[yes/no]> ";
//             std::getline(std::cin, value, '\n');
// 
//             if ((value != "yes") && (value != "no"))
//             {
//                 std::cout << "Choice is invalid, try again\n";
//                 continue;
//             }
//             else
//                 break;
//         }
//         while (true);
// 
//         ai->qt_gui_given = (value == "yes");
//     }
}

