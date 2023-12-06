#pragma once
#include "../globals.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <fstream>

#include "../globals.hpp"

namespace gpki {
struct profileInfo {
  /* PKI paths */
  std::string certs {};
  std::string keys {};
  std::string ca {};
  std::string reqs {};
  std::string serial {};
  /* config paths */
  std::string x509 {};
  std::string templates {};
  std::string openssl_config {};
  /* log path */
  std::string logs {};
};

/* Static class to manage the contents of the .profile file containing each profile and its root path */
class Profiles {
public:
  static inline std::map<std::string, std::string> _profiles{};
  static inline std::string_view _path{};
  static inline uint64_t _filesize{0};

  static int Initialize();
  static int Find(std::string_view key);
  static int Total();
  static int Add(std::string_view profile_name, std::string_view base_dir);
  static int Remove(std::string_view profile_name);
  static int Get(std::string_view profile_name, profileInfo &pinfo);
};
} // namespace gpki
