#pragma once
#include "../globals.hpp"
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

namespace gpki {
struct profileInfo {
  /* PKI paths */
  char *certs;
  char *keys;
  char *ca;
  char *reqs;
  char *serial;
  /* config paths */
  char *x509;
  char *templates;
  char *openssl_config;
  /* log path */
  char *logs;
};

class Profiles {
public:
  static inline std::map<std::string, std::string> _profiles{};
  static inline std::string_view _path{};
  static inline uint64_t _filesize{0};

  static int Initialize(std::string_view filepath);
  static int Find(std::string_view key);
  static int Total();
  static int Add(std::string_view profile_name, std::string_view base_dir);
  static int Remove(std::string_view profile_name);

  static int Get(std::string_view profile_name, profileInfo &pinfo);
};
} // namespace gpki
