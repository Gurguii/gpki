#pragma once
#include "../database/database.hpp"
#include "../globals.hpp"
#include "../pki/entities.hpp"
#include <fstream>
#include <iostream>
namespace gpki::entities {
std::string get_openssl_command();
std::string get_openssl_CA_command();
static int add();
} // namespace gpki::entities
