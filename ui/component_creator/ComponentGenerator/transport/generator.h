#pragma once

#include "global.h"
#include <map>
#include <string>

int generate_transport_component_code(const gengetopt_args_info *ai, std::map<std::string, std::string>& dict);

