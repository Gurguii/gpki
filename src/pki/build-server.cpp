#include "../profiles/profiles.hpp"
#include "utils.cpp"
#include <string_view>
namespace gpki {
int build_server(std::string_view profile_name) {
  if (!Profiles::Find(profile_name)) {
    return -1;
  }
  CertCreationInfo info;
  GetCertCreationInfo(profile_name, info, x509_server);
  printf("csr command -> %s\ncrt command -> %s\n", info.csr_command,
         info.crt_command);
  return -1;
  if (system(info.csr_command)) {
    return -1;
  }
  return system(info.crt_command);
}

int build_server(std::string_view profile_name, std::string_view common_name) {
  if (!Profiles::Find(profile_name)) {
    return -1;
  }
  profileInfo info;
  Profiles::Get(profile_name, info);
  char subj[512];
  memset(subj, 0, sizeof(subj));
  snprintf(subj, sizeof(subj), DEFAULT_SUBJECT, common_name.data());
  char command[1024];
  memset(command, 0, 1024);
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -out %s/%s-csr.pem "
           "-keyout %s/%s-key.pem -subj %s",
           info.openssl_config.c_str(), info.reqs.c_str(), common_name.data(),
           info.keys.c_str(), common_name.data(), subj);
  if (system(command)) {
    return -1;
  }
  return 0;
}
} // namespace gpki
