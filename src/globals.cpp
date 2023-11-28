#include "globals.hpp"

std::string globals::base_dir;
std::string globals::profiles_file;
std::string globals::config_dir;

void globals::Initialize(){
  globals::base_dir = std::filesystem::current_path();  
  globals::profiles_file = base_dir + "/.profiles";                 
  globals::config_dir = base_dir + "/config";
}
