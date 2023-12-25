#include "../globals.hpp"
#include "../profiles/sqlite3_db.hpp"
namespace gpki {

int build_ca() {
  for (const std::string &st : Globals::subopts) {
    std::cout << "subopt -> " << st << "\n";
  }
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile_name, info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s%sca-key.pem -out "
           "%s%sca-crt.pem -nodes",
           info.openssl_config, info.ca, SLASH, info.ca,
           SLASH);
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
}

int build_ca(const char *CN) {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile_name,info);
  char command[1024];
  snprintf(command, sizeof(command),
           "openssl req -config %s -new -x509 -keyout %s%s%s-key.pem -out "
           "%s%s%s-crt.pem -subj='/CN=%s' -nodes",
           info.openssl_config, info.keys, SLASH, CN,
           info.certs, SLASH, CN, CN);
  if (system(command)) {
    // fail
    return -1;
  };
  return 0;
};
} // namespace gpki
