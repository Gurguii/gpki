#include "../utils/file_utils.cpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../globals.hpp"
#include "../profiles/sqlite3_db.hpp"
namespace gpki {
int custom_sed(const char *tmplate,
               std::unordered_map<std::string, std::string> &keyvals,
               const char *outpath = nullptr) {
  // map meaning:
  // 'key -> pattern to match'
  // 'value -> new value'
  if (!std::filesystem::exists(tmplate)) {
    std::cout << "path " << tmplate << "doesn't exist\n";
    return -1;
  }
  std::ifstream template_file(tmplate);
  std::stringstream new_file{};
  std::string line{}, word{};
  while (getline(template_file, line)) {
    std::stringstream ss(line);
    while (ss >> word) {
      if (keyvals.find(word.c_str()) != keyvals.end()) {
        // key found - substitute with the value
        new_file << keyvals[word.c_str()] << " ";
      } else {
        new_file << word << " ";
      }
    }
    new_file << "\n";
  }
  if (outpath == nullptr) {
    std::cout << new_file.str() << "\n";
  } else {
    std::ofstream pkiconffile(outpath, std::ios::app);
    pkiconffile.write(new_file.str().c_str(), new_file.str().size());
  }
  return 0;
}

void pki_init() {
  // ask for profile_name and path making sure path is absolute (starts with
  // '/')
  std::string profile_name = "";
  std::string path = "";
  std::string input = "";

  do {
    std::cout << "[+] Please introduce desired profile name: ";
    std::getline(std::cin, input);
  } while (db::profile_exists(input));

  profile_name = input;

  do {
    std::cout << "[+] Please introduce pki base dir (absolute path): ";
    std::getline(std::cin, input);
  } while (!IS_ABSOLUT_PATH(input.c_str()));

  path = input;

  // Check that we have write permissions in such path
  if (!hasWritePermissions(path)) {
    std::cout << "[error] Not write permissions in '" << path << "'\n";
    return;
  };

  // Create PKI file structure
  ProfileInfo pinfo{
    .name = profile_name,
    .source_dir = path,
    .keys = path + SLASH + "pki" + SLASH  + "keys",
    .certs = path + SLASH + "pki" + SLASH + "certs",
    .ca = path + SLASH +  "pki" + SLASH + "ca",
    .reqs = path + SLASH +  "pki" + SLASH + "reqs",
    .serial = path + SLASH +  "pki" + SLASH + "serial",
    .x509 = path + SLASH + "config" + SLASH + "x509",
    .templates = path + SLASH +  "templates", 
    .openssl_config = path + SLASH +  "config" + SLASH + "gopenssl.cnf",
    .logs = path + SLASH +  "logs",
    .database = path + SLASH +  "pki" + SLASH + "database",
    .crl = path + SLASH + "pki"+ SLASH + "crl",
    .packs = path + SLASH + "packs"
  };

  std::string dstconfig = path + SLASH + "config";
  std::string sed_src = Globals::config_dir + SLASH + "gopenssl.cnf";
  std::string sed_dst = path + SLASH + "config" + SLASH + "gopenssl.cnf";
  // This function will create required directories / files and add profile to database
  db::insert_profile(pinfo,dstconfig);
  // This dirty code is to basicly ensure that the path saved to gopenssl.cnf
  // has '/' SLASHES no matter if it's windows or not cause its how openvpn wants it
#ifdef _WIN32
  for (char &c : path) {
    if (c == '\\') {
      c = '/';
    }
  }
#endif
  /* Substittute GPKI_BASEDIR in gopenssl.cnf template */
  std::unordered_map<std::string, std::string> _map{
      {"GPKI_BASEDIR", (path + "/pki")}};
  custom_sed(sed_src.c_str(), _map, sed_dst.c_str());
#ifdef _WIN32
  for (char &c : path) {
    if (c == '/') {
      c = '\\';
    }
  }
#endif
  if(Globals::prompt){
    std::cout << "Create dhparam and openvpn static key (tls-crypt)? Y/N: ";
    char c = getchar();
    if(c == 'y' || c == 'Y'){
      db::create_openvpn_static_key(pinfo.ca + SLASH + "ta.key");
      db::create_dhparam(pinfo.ca + SLASH + "dhparam" + std::to_string(Globals::keysize));
    }
  }
}
} // namespace gpki
