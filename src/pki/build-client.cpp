#include "../profiles/sqlite3_db.hpp"

namespace gpki {
int build_client() {
  ProfileInfo pinfo;
  CertCreationCommands commands;
  db::populate_ProfileInfo(Globals::profile_name,pinfo);
  db::populate_CertCreationCommands(&pinfo, Globals::profile_name, commands);
  if (system(commands.csr_command.c_str())) {
    return -1;
  }
  return system(commands.crt_command.c_str());
}
} // namespace gpki
