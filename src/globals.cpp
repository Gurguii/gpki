#include "globals.hpp"

std::string Globals::base_dir;
std::string Globals::profiles_file;
std::string Globals::config_dir;
std::string Globals::profile_name;

GPKI_ACTION Globals::action = ACTION_NONE;

void Globals::Initialize(int argc, const char **args) {
  Globals::base_dir = CURRENT_PATH;
  Globals::profiles_file = base_dir + SLASH + ".profiles";
  Globals::config_dir = base_dir + SLASH + ".." + SLASH + "config";
}
