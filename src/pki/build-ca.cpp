#include "../globals.hpp"
#include "../profiles/profiles.hpp"

namespace gpki {
int build_ca(std::string_view profile_name) {
  if (!Profiles::Find(profile_name)) {
    return -1;
  }
  profileInfo info;
  Profiles::Get(profile_name, info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s/ca-key.pem -out "
           "%s/ca-crt.pem -nodes",
           info.openssl_config.c_str(), info.keys.c_str(), info.certs.c_str());
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
}

int build_ca(std::string_view profile_name, const char *CN) {
  if (!Profiles::Find(profile_name)) {
    return -1;
  }
  profileInfo info;
  Profiles::Get(profile_name, info);
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
