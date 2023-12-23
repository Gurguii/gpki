#include "globals.hpp"

/* Initialized with Globals::Initialize() */
std::string Globals::base_dir;
std::string Globals::profiles_file;
std::string Globals::config_dir;
std::string Globals::profile_name;

/* Initialized by parser() */
std::vector<std::string> Globals::subopts;

/* Default values */
uint8_t Globals::prompt = 0;
int Globals::keysize = 2048;
std::string Globals::keyalgorithm = "rsa";
std::string Globals::outformat = "PEM";
std::string Globals::lasterror = "";

GPKI_ACTION Globals::action = ACTION_NONE;

void Globals::Initialize() {
  Globals::base_dir = CURRENT_PATH;
  Globals::profiles_file = base_dir + SLASH + ".profiles";
  Globals::config_dir = base_dir + SLASH + ".." + SLASH + "config";
}
