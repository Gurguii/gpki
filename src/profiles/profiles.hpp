#pragma once
#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "../globals.hpp"

namespace gpki{
class Profiles
{
  public:
    static inline std::map<std::string,std::string> _profiles{};
    static inline std::string_view _path{};
    static inline uint64_t _filesize{0};

    static int Initialize(std::string_view filepath);
    static int Find(std::string_view key);
    static int Total();
    /* NOT IMPLEMENTED */
    static int Add(std::string_view profile_name, std::string_view base_dir);
    static int Remove(std::string_view profile_name);
};
} // namespace gpki::profiles
