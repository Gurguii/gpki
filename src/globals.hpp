#pragma once
#include <filesystem>
#include <string>

class globals {
public:
  static std::string base_dir;
  static std::string profiles_file;
  static std::string config_dir;
  static void Initialize();
};
