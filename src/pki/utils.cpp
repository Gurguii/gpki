#include "../profiles/profiles.hpp"
#define DEFAULT_SUBJECT                                                        \
  "/C=ES/ST=CANARIAS/L=LAS PALMAS/O=MARIWANOS/CN=%s/emailAddress=noemail"

namespace gpki {
struct CertCreationInfo {
  char serial[10];
  char csr[512];
  char crt[512];
  char key[512];
  char csr_command[2048];
  char crt_command[2048];
};

enum class x509_type {
  client = 0x00,
#define x509_client x509_type::client
  server = 0x01
#define x509_server x509_type::server
};
int GetCertCreationInfo(std::string_view profile_name, CertCreationInfo &buff,
                        x509_type type) {
  profileInfo info;
  Profiles::Get(profile_name, info);
  for (char *st : {buff.serial, buff.csr, buff.crt, buff.key, buff.crt_command,
                   buff.csr_command}) {
    memset(st, 0, strlen(st));
  }
  std::ifstream(info.serial + "/serial").read(buff.serial, sizeof(buff.serial));
  snprintf(buff.csr, sizeof(buff.csr), "%s/%s-csr.pem", info.reqs.c_str(),
           buff.serial);
  snprintf(buff.crt, sizeof(buff.crt), "%s/%s-crt.pem", info.certs.c_str(),
           buff.serial);
  snprintf(buff.key, sizeof(buff.key), "%s/%s-key.pem", info.keys.c_str(),
           buff.serial);
  if (type == x509_server) {
    // csr/key creation command
    snprintf(buff.csr_command, sizeof(buff.csr_command),
             "openssl req -config %s -new -out %s -keyout %s -nodes",
             info.openssl_config.c_str(), buff.csr, buff.key);
    // crt signing command
    snprintf(buff.crt_command, sizeof(buff.crt_command),
             "openssl ca -config %s --notext -batch -in %s -out %s -extfile %s",
             info.openssl_config.c_str(), buff.csr, buff.crt,
             (info.x509 + "/server").c_str());
  } else if (type == x509_client) {
    // csr/key creation command
    snprintf(buff.csr_command, sizeof(buff.csr_command),
             "openssl req -config %s -new -out %s -keyout %s -nodes",
             info.openssl_config.c_str(), buff.csr, buff.key);
    // crt signing command
    snprintf(buff.crt_command, sizeof(buff.csr_command),
             "openssl ca -config %s --notext -batch -in %s -out %s -extfile %s",
             info.openssl_config.c_str(), buff.csr, buff.crt,
             (info.x509 + "/client").c_str());
  }
  return 0;
};
int GetCertCreationInfo(std::string_view profile_name, std::string_view CN,
                        CertCreationInfo &buff);
} // namespace gpki
