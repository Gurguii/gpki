#include "../profiles/sqlite3_db.hpp"
namespace gpki {

int build_ca() {
  for (const std::string &st : Globals::subopts) {
    std::cout << "subopt -> " << st << "\n";
  }
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile_name, info);
  std::string command = "openssl req -config " + info.openssl_config + " -new -x509 -keyout " + info.ca + SLASH + "ca-key.pem -out " + info.ca + SLASH + "ca-crt.pem -noenc";
  if (system(command.c_str())) {
    // fail
    return -1;
  };
  return 0;
}

int build_ca(const char *CN) {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile_name,info);
  std::string command = "openssl req -config " + info.openssl_config + "-new -x509 -keyout " + info.keys + SLASH + CN + "-key.pem -out " + info.certs + SLASH + CN + "-subj = '/CN=" + CN + "' -noenc"; 
  if (system(command.c_str())) {
    // fail
    return -1;
  };
  return 0;
};
} // namespace gpki
