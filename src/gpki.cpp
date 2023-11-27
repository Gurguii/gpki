#include <iostream>
#include <filesystem>
#include <map>

#include "parser.cpp"
#include "profiles/profiles.cpp"

std::string base_dir;
std::string profiles_file;
std::string config_dir;
Profiles profiles;

void InitializeVariables()
{
  base_dir = std::filesystem::current_path();  
  profiles_file = base_dir + "/.profiles";                 
  config_dir = base_dir + "/config";
}

int main(int argc, const char **args)
{
  //if(Parse(argc,args)){
  //  return -1;
  //};
  InitializeVariables();
  profiles.Initialize(profiles_file);
  std::cout << "removing entry 'path1'\n";
  profiles.Remove("path1");
  return 0;
}
