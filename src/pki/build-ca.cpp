#include "../globals.hpp"
#include "../profiles/profiles.hpp"
namespace gpki {

int build_ca() {
  profileInfo info;
  Profiles::Get(Globals::profile_name, info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s%sca-key.pem -out "
           "%s%sca-crt.pem -nodes",
           info.openssl_config.c_str(), info.ca.c_str(), SLASH, info.ca.c_str(),
           SLASH);
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
           "openssl req -config %s -new -x509 -keyout %s%s%s-key.pem -out "
           "%s%s%s-crt.pem -subj='/CN=%s' -nodes",
           info.openssl_config.c_str(), info.keys.c_str(), SLASH, CN,
           info.certs.c_str(), SLASH, CN, CN);
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
};
} // namespace gpki
