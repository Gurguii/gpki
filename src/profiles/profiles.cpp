#include "profiles.hpp"

bool hasWritePermissions(const std::filesystem::path &directoryPath) {
  /* this approach is kind of sad to see */
  try {
    auto s = std::filesystem::status(directoryPath);
    return true;
  } catch (std::exception &ex) {
    return false;
  }
}


using namespace gpki;
int profiles::create()
{
  do {
    std::cout << "[+] Please introduce desired profile name: ";
    std::getline(std::cin, PROFILE.name);
  } while (database::profiles::exists(PROFILE));

  do {
    std::cout << "[+] Please introduce pki base dir (absolute path): ";
    std::getline(std::cin, PROFILE.source_dir);
  } while (!IS_ABSOLUT_PATH(PROFILE.source_dir));

  // Check that we have write permissions in such path
  if (!hasWritePermissions(PROFILE.source_dir)) {
    seterror("[error] Not write permissions in '" + PROFILE.source_dir + "'\n");
    return -1;
  };
  
  // Create directories
  for(const std::string &relative : RELATIVE_DIRECTORY_PATHS){
    std::string path = PROFILE.source_dir + SLASH + relative;
    if(!std::filesystem::create_directories(path)){
      seterror("couldn't create directory " + path);
      // Remove the profile source dir
      std::filesystem::remove_all(PROFILE.source_dir);
    }
  }
  // Create files
  for(const std::pair<std::string,std::string> &p : RELATIVE_FILE_PATHS){
    // p.first -> path
    // p.second -> default file contents
    std::string path = PROFILE.source_dir + SLASH + p.first;
    std::ofstream(path,std::ios::app).write(p.second.c_str(),p.second.size());
    if(!std::filesystem::exists(path)){
      seterror("couldn't create file " + p.first);
      return -1;
    }
  }
  
  // Add profile to database 
  if(database::profiles::insert(PROFILE)){
    return -1;
  }

  // Create extra tls keys
  if (globals::behaviour::prompt) {
    std::cout << "Create dhparam and openvpn static key (tls-crypt)? Y/N: ";
    char c = getchar();
    if (c == 'y' || c == 'Y') {
      tls::create_openvpn_static_key(PROFILE.source_dir + SLASH + "tls" + SLASH + "ta.key");
      tls::create_dhparam(PROFILE.source_dir + SLASH + "tls" + SLASH + "dhparam" +
                     std::to_string(globals::tls::dhparam_keysize));
    }
  }
  return 0;
}
int profiles::remove()
{
  database::profiles::remove(PROFILE);
  std::filesystem::remove_all(PROFILE.source_dir);
  return 0;
}
