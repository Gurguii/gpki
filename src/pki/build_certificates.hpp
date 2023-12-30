#pragma once
#include "../defines.hpp"
#include "../profiles/sqlite3_db.hpp"
namespace gpki {
int build_ca();
int build_server();
int build_client();
} // namespace gpki
