#include "build_certificates.hpp"

using namespace gpki;

int gpki::build_ca() {
  ProfileInfo info;
  db::populate_ProfileInfo(Globals::profile.name, info);
  Globals::GetSubjectInfo();
  char st[Globals::subject_oneliner.size()];
  memcpy(st, &Globals::subject_oneliner[0], sizeof(st));
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
int gpki::build_server() {
  ProfileInfo pinfo;
  CertCreationCommands commands;
  db::populate_ProfileInfo(Globals::profile.name, pinfo);
  db::populate_CertCreationCommands(&pinfo, Globals::profile.name, commands);
  if (system(commands.csr_command.c_str())) {
    return -1;
  }
  return system(commands.crt_command.c_str());
};
int gpki::build_client() {
  ProfileInfo pinfo;
  CertCreationCommands commands;
  db::populate_ProfileInfo(Globals::profile.name, pinfo);
  db::populate_CertCreationCommands(&pinfo, Globals::profile.name, commands);
  if (system(commands.csr_command.c_str())) {
    return -1;
  }
  return system(commands.crt_command.c_str());
};
