#pragma once
#include "../profiles/sqlite3_db.hpp"

namespace gpki {
int build_ca();
int build_ca(const char *common_name);
} // namespace gpki
