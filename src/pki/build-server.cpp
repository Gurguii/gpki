#include "../globals.hpp"
#include "utils.hpp"
namespace gpki {
int build_server() {
  CertCreationInfo info;
  GetCertCreationInfo(Globals::profile_name, info, x509_server);
  if (system(info.csr_command)) {
    return -1;
  }
  return system(info.crt_command);
}

int build_server(std::string_view common_name) {
  profileInfo info;
  Profiles::Get(Globals::profile_name, info);
  char subj[512];
  memset(subj, 0, sizeof(subj));
  snprintf(subj, sizeof(subj), DEFAULT_SUBJECT, common_name.data());
  std::string command = "openssl req -config " + info.openssl_config +
                        "-new -out " + info.reqs + SLASH + common_name.data() +
                        " -keyout" + info.keys + SLASH + common_name.data();
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
} // namespace gpki
