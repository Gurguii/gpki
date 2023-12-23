#include "utils.hpp"
namespace gpki {
int GetCertCreationInfo(std::string_view profile_name, CertCreationInfo &buff,
                        x509_type type) {
  profileInfo info;
  Profiles::Get(profile_name, info);
  for (char *st : {buff.serial, buff.csr, buff.crt, buff.key, buff.crt_command,
                   buff.csr_command}) {
    memset(st, 0, strlen(st));
  }
  std::ifstream(info.serial + SLASH + "serial")
      .read(buff.serial, sizeof(buff.serial));
  // remove terminating \n from serial to be able to properly format the string
  buff.serial[strlen(buff.serial) - 1] = 0x00;
  snprintf(buff.csr, sizeof(buff.csr), "%s%s%s-csr.pem", info.reqs.c_str(),
           SLASH, buff.serial);
  snprintf(buff.crt, sizeof(buff.crt), "%s%s%s-crt.pem", info.certs.c_str(),
           SLASH, buff.serial);
  snprintf(buff.key, sizeof(buff.key), "%s%s%s-key.pem", info.keys.c_str(),
           SLASH, buff.serial);
  // printf("buff.csr -> %s\nbuff.crt -> %s\nbuff.key -> %s\n", buff.csr,
  // buff.crt,buff.key);
  if (type == x509_server) {
    // SERVER
    // csr/key creation command
    snprintf(buff.csr_command, sizeof(buff.csr_command),
             "openssl req -config %s -newkey %i:%s -outform %s -out %s -keyout %s -noenc",
             info.openssl_config.c_str(), Globals::keysize, Globals::keyalgorithm.c_str(),Globals::outformat.c_str(), buff.csr, buff.key);
    // crt signing command
    snprintf(buff.crt_command, sizeof(buff.crt_command),
             "openssl ca -config %s %s -in %s -out %s -extfile %s %s",
             info.openssl_config.c_str(), Globals::outformat.c_str(), buff.csr, buff.crt,
             (info.x509 + SLASH + "server").c_str(), Globals::prompt ? "\x00" : "--notext -batch");
  } else if (type == x509_client) {
    // CLIENT
    // csr/key creation command
    snprintf(buff.csr_command, sizeof(buff.csr_command),
             "openssl req -config %s -newkey %i:%s -outform %s -out %s -keyout %s -noenc",
             info.openssl_config.c_str(), Globals::keysize, Globals::keyalgorithm.c_str(), Globals::outformat.c_str(), buff.csr, buff.key);
    // crt signing command
    snprintf(buff.crt_command, sizeof(buff.csr_command),
             "openssl ca -config %s -in %s -out %s -extfile %s %s",
             info.openssl_config.c_str(), buff.csr, buff.crt,
             (info.x509 + SLASH + "client").c_str(), Globals::prompt ? "\x00" : "--notext -batch");
  }
  return 0;
};
int GetCertCreationInfo(std::string_view profile_name, CertCreationInfo &buff,
                        x509_type type, std::string_view CN) {
  // TODO - IMPLEMENT THIS
  return 0;
};
} // namespace gpki
