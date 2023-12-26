#include "../profiles/sqlite3_db.hpp"

namespace gpki {
int build_server() {
  ProfileInfo pinfo;
  CertCreationCommands commands;
  db::populate_ProfileInfo(Globals::profile_name,pinfo);
  db::populate_CertCreationCommands(&pinfo, Globals::profile_name, commands);
  if (system(commands.csr_command.c_str())) {
    return -1;
  }
  return system(commands.crt_command.c_str());
}

int build_server(std::string_view common_name) {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile_name,info);
  char subj[512];
  memset(subj, 0, sizeof(subj));
  snprintf(subj, sizeof(subj), DEFAULT_SUBJECT, common_name.data());
  std::string command = "openssl req -config " + std::string(info.openssl_config) +
                        "-new -out " + std::string(info.reqs) + SLASH + common_name.data() +
                        " -keyout" + std::string(info.keys) + SLASH + common_name.data();
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
} // namespace gpki
