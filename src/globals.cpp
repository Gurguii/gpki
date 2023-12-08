#include "globals.hpp"

std::string Globals::base_dir;
std::string Globals::profiles_file;
std::string Globals::config_dir;
std::string Globals::profile_name;

GPKI_ACTION Globals::action = ACTION_NONE;

void Globals::Initialize(int argc, const char **args) {
  Globals::base_dir = std::filesystem::current_path();
  Globals::profiles_file = base_dir + "/.profiles";
  Globals::config_dir = base_dir + "/../config";
}
