#pragma once
#include "../profiles/sqlite3_db.hpp"

namespace gpki {
int build_server();
int build_server(std::string_view CN);
} // namespace gpki
