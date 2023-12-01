#include "../globals.hpp"
#include "../profiles/profiles.hpp"
namespace gpki {
int build_ca(std::string_view profile_name) {
  if (Profiles::Find(profile_name)) {
    return -1;
  }
  profileInfo info;
  Profiles::Get(profile_name, info);

  printf("profile data -> %s %s %s %s %s %s\n", info.ca, info.ca, info.logs,
         info.openssl_config, info.certs, info.keys);
  return 0;
}
int build_ca(std::string_view profile_name, const char *CN) {
  if (Profiles::Find(profile_name)) {
    return -1;
  }
  return 0;
};
} // namespace gpki
