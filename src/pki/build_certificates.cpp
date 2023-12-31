#include "build_certificates.hpp"

using namespace gpki;

int gpki::build_ca() {
  Globals::GetCertInfo();
  char st[Globals::subject_oneliner.size()];
  memcpy(st, &Globals::subject_oneliner[0], sizeof(st));
  std::string command =
      "openssl req -config " + Globals::profile.openssl_config +
      " -new -x509 -keyout " + Globals::profile.ca + SLASH +
      "ca-key.pem -out " + Globals::profile.ca + SLASH + "ca-crt.pem -subj '";
  command += std::string(st) + "' -noenc";
  if (system(command.c_str())) {
    // fail
    Globals::lasterror = "system() command failed";
    return -1;
  };
  // Insert entity
  Globals::InsertEntity();
  return 0;
}
int gpki::build_server() {
  Globals::GetCertInfo();
  if (system(Globals::certinfo.commands.csr_command.c_str())) {
    return -1;
  }
  // Insert entity
  Globals::InsertEntity();
  return system(Globals::certinfo.commands.crt_command.c_str());
};
int gpki::build_client() {
  Globals::GetCertInfo();
  ProfileInfo pinfo;
  if (system(Globals::certinfo.commands.csr_command.c_str())) {
    return -1;
  }
  // Insert entity
  Globals::InsertEntity();
  return system(Globals::certinfo.commands.crt_command.c_str());
};
