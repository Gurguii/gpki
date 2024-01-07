#pragma once
#include "../entities/entities.hpp"
#include "../profiles/profiles.hpp"
#include <fstream>
#include <string>
#include <string_view>
namespace gpki::pki {
int build_ca();
int build_server();
int build_client();
std::string update_serial();
int get_serial();
} // namespace gpki::pki
