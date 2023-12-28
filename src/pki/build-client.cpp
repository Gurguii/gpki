#include "build-client.hpp"
namespace gpki {
int build_client() {
  ProfileInfo pinfo;
  CertCreationCommands commands;
  db::populate_ProfileInfo(Globals::profile.name, pinfo);
  db::populate_CertCreationCommands(&pinfo, Globals::profile.name, commands);
  if (system(commands.csr_command.c_str())) {
    return -1;
  }
  return system(commands.crt_command.c_str());
}
} // namespace gpki
