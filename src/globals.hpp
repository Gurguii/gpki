#pragma once
#include "defines.hpp"
#include "pki/security_key_creation.hpp"
#include "profiles/sqlite3_db.hpp"
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

enum class _pki_relative_paths : uint8_t {
  templates = 0x01,
#define RPATH_TEMPLATES (uint8_t) _pki_relative_paths::templates
  packs = 0x02,
#define RPATH_PACKS (uint8_t) _pki_relative_paths::packs
  ca = 0x03,
#define RPATH_CA (uint8_t) _pki_relative_paths::ca
  certs = 0x04,
#define RPATH_CERTS (uint8_t) _pki_relative_paths::certs
  keys = 0x05,
#define RPATH_KEYS (uint8_t) _pki_relative_paths::keys
  reqs = 0x06,
#define RPATH_REQS (uint8_t) _pki_relative_paths::reqs
  crl = 0x07,
#define RPATH_CRL (uint8_t) _pki_relative_paths::crl
  database = 0x08,
#define RPATH_DATABASE (uint8_t) _pki_relative_paths::database
  serial = 0x09,
#define RPATH_SERIAL (uint8_t) _pki_relative_paths::serial
  logs = 0xa0
#define RPATH_LOGS (uint8_t) _pki_relative_paths::logs
};
namespace gpki {

class Globals {
public:
  static inline std::unordered_map<uint8_t, std::string>
      pki_relative_directory_paths{
          {RPATH_TEMPLATES, "templates"}, {RPATH_PACKS, "packs"},
          {RPATH_CA, "pki/ca"},           {RPATH_CERTS, "pki/certs"},
          {RPATH_KEYS, "pki/keys"},       {RPATH_REQS, "pki/reqs"},
          {RPATH_CRL, "pki/crl"},         {RPATH_DATABASE, "pki/database"},
          {RPATH_SERIAL, "pki/serial"},   {RPATH_LOGS, "logs"}};

  /* Metainfo */
  static std::string base_dir;
  static std::string profiles_db;
  static std::string config_dir;

  /* Subopts - modify function behaviour e.g when creating new certificates,
   * change the default keysize */
  static std::vector<std::string> subopts;

  /* Behaviour variables */
  static inline uint8_t verbose = 1;
  static inline uint8_t prompt = 1;

  /* Current profile */
  static inline ProfileInfo profile{};

  /* Subject info */
  static inline EntityInfo certinfo{.subject{.country = "ES",
                                             .state = "CANARIAS",
                                             .location = "LAS PALMAS",
                                             .organisation = "MARIWANOS",
                                             .cn = "",
                                             .email = "default@example.com"}};

  static inline std::string subject_oneliner = std::string(2048, '\x00');
  /* Action variable - pointer to action function, e.g build_client()
   * build_ca() display_profiles() */
  static inline int (*action)() = nullptr;

  /* PKI variables */

  static inline uint8_t x509_extension = X509_NONE;

  /* User modifiable options */
  static inline int keysize = 2048;
  static inline std::string keyalgorithm = "rsa";
  static inline std::string outformat = "PEM";
  // PKI extra security keys params
  static inline int dhparam_keysize = 1024;
  static inline int tls_keysize = 1024;

  /* Last error message */
  static inline std::string lasterror = "no errors yet";

  // OPTIONS

  /* Methods */
  static void Initialize();

  static int GetProfileInfo();
  static int GetCertInfo();

  static int InsertProfile();
  static int InsertEntity();
  static int CreateFiles();
  static int DeleteProfile();
  static int SelectProfile();

  static int PopulateProfileInfo();
};
} // namespace gpki
