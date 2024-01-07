#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include "../globals.hpp"
#include "../defines.hpp"
#include "../database/database.hpp"
#include "../pki/tls.hpp"

#define RELATIVE_DIRECTORY_PATHS std::vector<std::string>{"tls","templates","pki" + SLASH + "ca","pki" + SLASH + "keys","pki" + SLASH + "crl","pki" + SLASH + "serial","packs","pki" + SLASH + "certs","pki" + SLASH + "reqs","pki" + SLASH + "database","logs"}
#define RELATIVE_FILE_PATHS std::unordered_map<std::string,std::string>{{"pki" + SLASH + "crl" + SLASH + "crlnumber","1000"},{"pki" + SLASH + "serial" + SLASH + "serial","01"},{"pki" + SLASH + "database" + SLASH + "index.txt",""}}
namespace gpki::profiles
{
static int create(); // uses globals::profile
static int remove();
static int populate();
}
