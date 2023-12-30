#pragma once
#ifdef _WIN32
#define GPKI_MAX_PATH MAX_PATH
#else
#define GPKI_MAX_PATH 4096
#include "../globals.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <unordered_map>

#define DEFAULT_CRLNUMBER "1000"
#define DEFAULT_SERIAL "01"

struct CertCreationCommands {
  std::string csr_command{};
  std::string crt_command{};
};
namespace gpki {
class db {
private:
  /* Properties */
  static const char *db_path;
  static inline std::string lasterror = "no errors yet";
  static sqlite3 *_db;
  /* Methods */
  static int open_db();
  static int close_db();

public:
  // Constructor
  static int initialize(const char *dbpath);
  // Utils
  static int create_pki_directory_structure(ProfileInfo *pinfo);

  // Crud
  static int insert_profile(ProfileInfo &pinfo);
  static int insert_certificate(ProfileInfo &pinfo, Subject &subj);
  static int select_profile(std::string_view profile_name);
  static int delete_profile(std::string_view profile_name);
  static int
  update_database(std::string_view profile_name,
                  std::unordered_map<std::string, std::string> values);

  // Printing
  static int display_profiles();
  static const std::string &get_error();

  // Structure population
  static int populate_CertCreationCommands(ProfileInfo *ptr,
                                           std::string_view profile_name,
                                           CertCreationCommands &buff);
  static int populate_ProfileInfo(std::string_view profile_name,
                                  ProfileInfo &buff);

  // Checks
  static int profile_exists(std::string_view profile_name);

  // Extra security
  /* create dhparam */
  static int create_dhparam(std::string_view outpath);
  /* create openvpn static key */
  static int create_openvpn_static_key(std::string_view outpath);
};
} // namespace gpki
#endif
