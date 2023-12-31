#pragma once
#include "../defines.hpp"
#include "../globals.hpp"
#include <cstdlib>
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
#define TABLES                                                                 \
  { "profiles", "certificates" }

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

  // Insert
  static int insert_profile(ProfileInfo &pinfo);
  static int insert_entity(EntityInfo &cinfo);

  // Select
  static int select_profile(std::string_view profile_name);

  // Delete
  static int delete_profile(std::string_view profile_name);
  static int delete_entity(std::string_view cn);
  static int delete_all_profiles();
  // Update
  static int
  update_database(std::string_view profile_name,
                  std::unordered_map<std::string, std::string> values);

  // Printing
  static int display_profiles();
  static const std::string &get_error();

  // Structure population
  static int populate_EntityInfo(ProfileInfo *ptr, EntityInfo &buff);
  static int populate_ProfileInfo(std::string_view profile_name,
                                  ProfileInfo &buff);

  // Checking
  static int profile_exists(std::string_view profile_name);
  static int profile_exists(int profile_id);
  static int entity_exists(std::string &cn);
  static int entity_exists(int certificate_id);

  // Extra security
  /* create dhparam */
  static int create_dhparam(std::string_view outpath);
  /* create openvpn static key */
  static int create_openvpn_static_key(std::string_view outpath);
};
} // namespace gpki
