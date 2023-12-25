#include "globals.hpp"

/* Initialized with Globals::Initialize() */
std::string Globals::base_dir;
std::string Globals::config_dir;
std::string Globals::profile_name;
std::string Globals::profiles_db;

/* Initialized by parser() */
std::vector<std::string> Globals::subopts;

/* Behaviour variables */
uint8_t Globals::verbose = 0;
uint8_t Globals::prompt = 1;

/* PKI variables */
int Globals::keysize = 2048;
std::string Globals::keyalgorithm = "rsa";
std::string Globals::outformat = "PEM";

/* Last error message */
std::string Globals::lasterror = "";
GPKI_ACTION Globals::action = ACTION_NONE;

void Globals::Initialize() {
  Globals::base_dir = CURRENT_PATH;
  Globals::profiles_db = base_dir + SLASH + ".gpki.db";
  Globals::config_dir = base_dir + SLASH + ".." + SLASH + "config";
}
