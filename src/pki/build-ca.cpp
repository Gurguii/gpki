#include "build-ca.hpp"

namespace gpki {

int build_ca() {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile.name, info);
  Globals::GetSubjectInfo();
  char st[Globals::subject_oneliner.size()];
  memcpy(st, &Globals::subject_oneliner[0], sizeof(st));
  /*
  std::string command =
      "openssl req -config " + info.openssl_config + " -new -x509 -keyout " +
      info.ca + SLASH + "ca-key.pem -out " + info.ca + SLASH +
      "ca-crt.pem -subj '" + Globals::subject_oneliner + "' -noenc";
  */
  std::string command = "openssl req -config " + info.openssl_config +
                        " -new -x509 -keyout " + info.ca + SLASH +
                        "ca-key.pem -out " + info.ca + SLASH +
                        "ca-crt.pem -subj '";
  command += std::string(st) + "' -noenc";
  if (system(command.c_str())) {
    // fail
    Globals::lasterror = "system() command failed";
    return -1;
  };
  return 0;
}

int build_ca(const char *CN) {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile.name, info);
  std::string command = "openssl req -config " + info.openssl_config +
                        "-new -x509 -keyout " + info.keys + SLASH + CN +
                        "-key.pem -out " + info.certs + SLASH + CN +
                        "-subj = '/CN=" + CN + "' -noenc";
  if (system(command.c_str())) {
    // fail
    return -1;
  };
  return 0;
};
} // namespace gpki
