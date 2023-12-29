#include "build-ca.hpp"

namespace gpki {

int build_ca() {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile.name, info);
  Globals::GetSubjectInfo();
  std::string command = "openssl req -config " + info.openssl_config +
                        " -new -x509 -keyout " + info.ca + SLASH +
                        "ca-key.pem -out " + info.ca + SLASH +
                        Globals::subject.cn + "ca-crt.pem -subj '" +
                        Globals::subject_oneliner + "' -noenc";
  std::cout << "command -> " << command << "\n";
  if (system(command.c_str())) {
    // fail
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
