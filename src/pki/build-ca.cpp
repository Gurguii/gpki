#include "../profiles/profiles.hpp"
#include "../globals.hpp"
namespace gpki {

int build_ca() {
  profileInfo info;
  Profiles::Get(Globals::profile_name, info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s/ca-key.pem -out "
           "%s/ca-crt.pem -nodes",
           info.openssl_config.c_str(), info.ca.c_str(), info.ca.c_str());
  printf("command -> %s\n", command);
  return -1;
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
}

int build_ca(const char *CN) {
  if (!Profiles::Find(Globals::profile_name)) {
    return -1;
  }
  profileInfo info;
  Profiles::Get(Globals::profile_name, info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s/%s-key.pem -out "
           "%s/%s-crt.pem -subj='/CN=%s' -nodes",
           info.openssl_config.c_str(), info.keys.c_str(), CN,
           info.certs.c_str(), CN, CN);
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
};
} // namespace gpki
