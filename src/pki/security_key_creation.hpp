#pragma once
#include "../globals.hpp"
#include <iostream>
#include <string_view>
namespace gpki {
int create_dhparam(std::string_view outpath);
int create_openvpn_static_key(std::string_view outpath);
} // namespace gpki
