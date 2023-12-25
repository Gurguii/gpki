#pragma once
#include "../globals.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <sqlite3.h>
#include "../globals.hpp"

struct profileInfo
{
  char *name;
  char *keys;
  char *certs;
  char *ca;
  char *reqs;
  char *serial;
  char *x509;
  char *templates;
  char *openssl_config;
  char *logs;
};
namespace gpki {
/* Static class to manage the contents of the .profile file containing each
 * profile and its root path */
class PlaintextDB {
public:
  static inline std::map<std::string, std::string> _profiles{};
  static inline std::string_view _path{};
  static inline uint64_t _filesize{0};

  /* Initializes the _profiles, filling it with contents from .profiles
   */
  static int Initialize();
  /* Return 1 if profile is found else 0 */
  static int Find(std::string_view key);
  /* Return amount of profiles */
  static int Total();
  /* Add profile with name 'profile_name' - add entry to .profiles and key to
   * _profiles map */
  static int Add(std::string_view profile_name, std::string_view base_dir);
  /* Remove profile with name 'profile_name' - source dir and entry from
   * .profiles */
  static int Remove(std::string_view profile_name);
  /* Fill pinfo structure with info about profile with name 'profile_name' */
  static int Get(std::string_view profile_name, profileInfo &pinfo);
  /* Print .profiles file */
  static int List();
  /* Show formatted info about a profile */
  static int ShowInfo(std::string_view profile_name);
};
} // namespace gpki
