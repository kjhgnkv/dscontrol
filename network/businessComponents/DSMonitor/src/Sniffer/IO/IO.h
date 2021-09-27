#pragma once

#include <string>
#include <vector>

namespace sockets
{
class detail_view;
}

namespace IO
{
void print_help();

void print(const std::string &line);
void println(const std::string &line);

void print_err(const std::string &line);

void print_socket(const std::vector<sockets::detail_view> &view);
void print_socket(const sockets::detail_view &view);
}
