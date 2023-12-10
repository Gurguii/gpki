#include "../profiles/profiles.hpp"
#include "../utils/file_utils.cpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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
  // the map will be something like
  // 'key -> pattern to match'
  // 'value -> new value'
  if (!std::filesystem::exists(tmplate)) {
    printf("path %s doesn't exist\n", tmplate);
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
  } while (Profiles::Find(input));

  profile_name = input;

  do {
    std::cout << "[+] Please introduce pki base dir (absolute path): ";
    std::getline(std::cin, input);
  } while (!IS_ABSOLUT_PATH(input.c_str()));
  path = std::move(input);

  // Check that we have write permissions in such path
  if (!hasWritePermissions(path)) {
    std::cout << "[error] Not write permissions in '" << path << "'\n";
    return;
  };
  // Create PKI structure

  // 1. create directories
  for (const char *&dir : pki_structure_relative_directory_paths) {
    if (!std::filesystem::create_directories(path + SLASH + dir)) {
      // This shouldn't happen
      std::cout << "[FAIL] Create directory '" << dir << "' failed\n";
      // TODO - add cleanup function to delete what's been done
      // e.g remove globals::base_dir
    };
  }
  // 2. create files
  std::ofstream(path + SLASH + "pki" + SLASH + "crl" + SLASH + "crlnumber")
      .write(STARTING_CRLNUMBER, strlen(STARTING_CRLNUMBER));
  std::ofstream(path + SLASH + "pki" + SLASH + "serial" + SLASH + "serial")
      .write(STARTING_SERIAL, strlen(STARTING_SERIAL));
  std::ofstream(path + SLASH + "pki" + SLASH + "database" + SLASH +
                "index.txt");

  std::string dstconfig = path + SLASH + "config";
  std::string sed_src = Globals::config_dir + SLASH + "gopenssl.cnf";
  std::string sed_dst = path + SLASH + "config" + SLASH + "gopenssl.cnf";
  // copy config directory to path
  std::filesystem::copy(Globals::config_dir.c_str(), dstconfig.c_str());
  // substitute required values in the config file
  std::unordered_map<std::string, std::string> _map{
      {"GPKI_BASEDIR", (path + SLASH + "pki")}};
  custom_sed(sed_src.c_str(), _map, sed_dst.c_str());

  // 3. create .pkiconf file
  // auto contents = new std::string(4096 * 4, '\x00');
  char *pkifilecontents = (char *)malloc(16000);
  memset(pkifilecontents, 0, 16000);
  if (pkifilecontents == nullptr) {
    std::cerr << "Couldn't allocate memory for .pkiconf\n";
    return;
  }
  snprintf(pkifilecontents, 16000, pkiconf_template.data(), path.c_str(),
           profile_name.c_str(), path.c_str(), path.c_str(), path.c_str(),
           path.c_str(), path.c_str(), path.c_str(), path.c_str(), path.c_str(),
           path.c_str());
  std::string pkiconf_path = path + SLASH + "config" + SLASH + ".pkiconf";
  std::ofstream(pkiconf_path).write(pkifilecontents, 16000);
  free(pkifilecontents);
  pkifilecontents = nullptr;

  // 4. Add profile to .profiles
  Profiles::Add(profile_name, path);
}
} // namespace gpki
