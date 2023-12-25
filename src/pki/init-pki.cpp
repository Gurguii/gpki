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
#ifdef _WIN32
std::string pkiconf_template = R"(
# Auto generated config file by gpki
# Author: Airán 'Gurguii' Gómez

# Profile stuff
profile_source_dir=%s
profile_name=%s

# Openssl configuration file
openssl_config_file=%s\config\gopenssl.cnf

# PKi paths
certs=%s\pki\certs
keys=%s\pki\keys
ca=%s\pki\ca
reqs=%s\pki\reqs
serial=%s\pki\serial

# Contains files with extensions for each certificate (client/server)
x509_dir=%s\config\x509

# Contains template files used to create packs or configuration files
templates=%s\config\templates

# Logs
logs=%s\logs
)";
#else
std::string pkiconf_template = R"(
# Auto generated config file by gpki
# Author: Airán 'Gurguii' Gómez

# Profile stuff
profile_source_dir=%s
profile_name=%s

# Openssl configuration file
openssl_config_file=%s/config/gopenssl.cnf

# PKi paths
certs=%s/pki/certs
keys=%s/pki/keys
ca=%s/pki/ca
reqs=%s/pki/reqs
serial=%s/pki/serial

# Contains files with extensions for each certificate (client/server)
x509_dir=%s/config/x509

# Contains template files used to create packs or configuration files
templates=%s/config/templates

# Logs
logs=%s/logs
)";
#endif
std::vector<const char *> pki_structure_relative_directory_paths{
    "x509",       "templates", "packs",    "pki/ca",
    "pki/crl",    "pki/certs", "pki/keys", "pki/database",
    "pki/serial", "pki/reqs",  "logs"};

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
  } while (db::profile_exists(profile_name));

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
    .keys = path + std::string(profile_name) + SLASH + "pki" + SLASH  + "keys",
    .certs = path + std::string(profile_name) + SLASH + "pki" + SLASH + "certs",
    .ca = path + std::string(profile_name) + SLASH + "pki" + SLASH + "ca",
    .reqs = path + std::string(profile_name) + SLASH + "pki" + SLASH + "reqs",
    .serial = path + std::string(profile_name) + SLASH + "pki" + SLASH + "serial",
    .x509 = path + std::string(profile_name) + SLASH + "x509",
    .templates = path + std::string(profile_name) + SLASH + "templates", 
    .openssl_config = path + std::string(profile_name) + SLASH + "config" + SLASH + "gopenssl.cnf",
    .logs = path + std::string(profile_name) + SLASH + "logs",
    .database = path + std::string(profile_name) + SLASH + "database",
    .crl = path + std::string(profile_name) + SLASH + "crl"
  };

  std::string dstconfig = path + SLASH + "config";
  std::string sed_src = Globals::config_dir + SLASH + "gopenssl.cnf";
  std::string sed_dst = path + SLASH + "config" + SLASH + "gopenssl.cnf";
  // This function will create required directories / files and add profile to database
  db::insert_profile(pinfo,sed_src,sed_dst);
  // This dirty code is to basicly ensure that the path saved to gopenssl.cnf
  // has '/' SLASHES no matter if it's windows or not
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
}
} // namespace gpki
