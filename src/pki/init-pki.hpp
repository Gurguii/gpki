#pragma once
#include "../globals.hpp"
#include "../profiles/sqlite3_db.hpp"
#include "../utils/file_utils.cpp"
#include "security_key_creation.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
namespace gpki {
int pki_init();
}
