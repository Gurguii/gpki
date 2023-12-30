#pragma once
#include <cstdint>
#include <iostream>
struct ProfileInfo {
  int id;
  std::string name{};
  std::string source_dir{};
  std::string keys{};
  std::string certs{};
  std::string ca{};
  std::string reqs{};
  std::string serial{};
  std::string x509{};
  std::string templates{};
  std::string openssl_config{};
  std::string logs{};
  std::string database{};
  std::string crl{};
  std::string packs{};
};

struct Subject {
  std::string country;
  std::string state;
  std::string location;
  std::string organisation;
  std::string cn;
  std::string email;
};

#define STARTING_CRLNUMBER "1000\n"
#define STARTING_SERIAL "01\n"
#define SUBJECT_TEMPLATE "/C=%s/ST=%s/L=%s/O=%s/CN=%s/emailAddress=%s"

const auto IS_ABSOLUT_PATH = [](const char *path) {
#ifdef _WIN32
  return std::isalpha(*path);
#else
  return (*path == '/');
#endif
};

#ifdef _WIN32
/* WINDOWS STUFF */
#define CURRENT_PATH std::filesystem::current_path().string()
#define SLASH "\\"
#else
/* LINUX STUFF */
#define CURRENT_PATH std::filesystem::current_path()
#define SLASH "/"
#endif

enum class ACTION : uint8_t {
#define GPKI_ACTION uint8_t
  none = 0x00,
/* PKI STUFF */
#define ACTION_NONE static_cast<uint8_t>(ACTION::none)
  init_pki = 0x01,
#define ACTION_INIT_PKI static_cast<uint8_t>(ACTION::init_pki)
  build_ca = 0x03,
#define ACTION_BUILD_CA static_cast<uint8_t>(ACTION::build_ca)
  build_server = 0x05,
#define ACTION_BUILD_SERVER static_cast<uint8_t>(ACTION::build_server)
  build_client = 0x07,
#define ACTION_BUILD_CLIENT static_cast<uint8_t>(ACTION::build_client)
  remove_profile = 0x09,
/* PROFILE STUFF */
#define ACTION_PROFILE_REMOVE static_cast<uint8_t>(ACTION::remove_profile)
  list_profiles = 0x0b,
#define ACTION_PROFILE_LIST static_cast<uint8_t>(ACTION::list_profiles)
  profile_info = 0x0d,
#define ACTION_PROFILE_INFO static_cast<uint8_t>(ACTION::profile_info)
  /* CRL STUFF */
  revoke_certificate = 0x0f,
#define ACTION_REVOKE_CERTIFICATE                                              \
  static_cast<uint8_t>(ACTION::revoke_certificate)
  generate_crl = 0x11,
#define ACTION_GENERATE_CRL static_cast<uint8_t>(ACTION::generate_crl)
};

enum class KEYTYPE : uint8_t {
#define GPKI_KEYTYPE uint8_t
  pem = 0x01,
#define KEYTYPE_PEM static_cast<uint8_t>(KEYTYPE::pem)
  der = 0x03,
#define KEYTYPE_DER static_cast<uint8_t>(KEYTYPE::der)
};

enum class X509_EXT : uint8_t {
#define GPKI_X509 uint8_t
  none = 0x00,
#define X509_NONE static_cast<uint8_t>(X509_EXT::none)
  client = 0x01,
#define X509_CLIENT static_cast<uint8_t>(X509_EXT::client)
  server = 0x03
#define X509_SERVER static_cast<uint8_t>(X509_EXT::server)
};

enum class X509_FORM : uint8_t {
#define GPKI_X509_FORMAT uint8_t
  PEM = 0x01,
#define X509_PEM static_cast<uint8_t>(X509_FORM::PEM)
  DER = 0x03
#define X509_DER static_cast<uint8_t>(X509_FORM::DER)
};
