#pragma once
#define GPKI_UTILS
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
  client = 0x01,
#define x509_client x509_type::client
  server = 0x02
#define x509_server x509_type::server
};
int GetCertCreationInfo(std::string_view profile_name, CertCreationInfo &buff,
                        x509_type type);
int GetCertCreationInfo(std::string_view profile_name, CertCreationInfo &buff,
                        x509_type type, std::string_view CN);
} // namespace gpki
