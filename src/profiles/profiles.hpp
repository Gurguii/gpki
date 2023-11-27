#pragma once
#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstring>
class Profiles
{
  private:
    static inline std::map<std::string,std::string> _profiles{};
    static inline std::string_view _path{};
    static inline uint64_t _filesize{0};
  public:
    int Initialize(std::string_view filepath);
    int Find(std::string_view key);
    int Total();
    /* NOT IMPLEMENTED */
    int Add(std::string_view profile_name);
    int Add(std::string_view profile_name, std::string_view base_dir);
    int Remove(std::string_view profile_name);
};
