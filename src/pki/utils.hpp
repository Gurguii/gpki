#pragma once
#define GPKI_UTILS
#include "../globals.hpp"
#include "../profiles/sqlite3_db.hpp"
#include <cstring>
#include <fstream>

#define DEFAULT_SUBJECT                                                        \
  "/C=ES/ST=CANARIAS/L=LAS PALMAS/O=MARIWANOS/CN=%s/emailAddress=noemail"

namespace gpki {
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
