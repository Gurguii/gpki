#include "pki-init.hpp"
#define STARTING_CRLNUMBER "1000\n"
#define STARTING_SERIAL "01\n"

namespace gpki {
std::string pkiconf_template = R"(
# Auto generated config file by gpki
# Author: Airán 'Gurguii' Gómez

# Profile stuff
profile_source_dir=%s
profile_name=%s

# Openssl configutation file
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

std::vector<const char *> pki_structure_relative_directory_paths{
    "x509",      "templates", "packs",        "pki/ca",     "pki/crl",
    "pki/certs", "pki/keys",  "pki/database", "pki/serial", "logs"};

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
  } while (input[0] != '/');
  path = std::move(input);

  // Check that we have write permissions in such path
  if (!hasWritePermissions(path)) {
    std::cout << "[error] Not write permissions in target path\n";
    return;
  };
  // Create PKI structure

  // 1. create directories
  for (const char *&dir : pki_structure_relative_directory_paths) {
    if (!std::filesystem::create_directories(path + "/" + dir)) {
      // This shouldn't happen
      std::cout << "[FAIL] Create directory '" << dir << "' failed\n";
      // TODO - add cleanup function to delete what's been done
      // e.g remove globals::base_dir
    };
  }

  // 2. create files
  std::ofstream(path + "/pki/crl/crlnumber")
      .write(STARTING_CRLNUMBER, strlen(STARTING_CRLNUMBER));
  std::ofstream(path + "/pki/serial/serial")
      .write(STARTING_SERIAL, strlen(STARTING_SERIAL));
  std::ofstream(path + "/pki/database/index.txt");
  char command[120];
  memset(command, 0, sizeof(command));
  snprintf(command, sizeof(command), "cp -rf %s %s/config",
           globals::config_dir.c_str(), path.c_str());
  system(command);
  memset(command, 0, sizeof(command));
  snprintf(command, sizeof(command), "sed -i 's#GPKI_BASEDIR#%s/pki#' %s",
           path.c_str(), (path + "/config/gopenssl.cnf").c_str());
  system(command);
  memset(command, 0, sizeof(command));

  // 3. create .pkiconf file
  // auto contents = new std::string(4096 * 4, '\x00');
  char *pkifilecontents = (char *)malloc(16000);
  memset(pkifilecontents, 0, 16000);
  if (pkifilecontents == nullptr) {
    return;
  }
  snprintf(pkifilecontents, 16000, pkiconf_template.data(), path.c_str(),
           profile_name.c_str(), path.c_str(), path.c_str(), path.c_str(),
           path.c_str(), path.c_str(), path.c_str(), path.c_str(), path.c_str(),
           path.c_str());

  std::ofstream(path + "/config/.pkiconf").write(pkifilecontents, 16000);
  free(pkifilecontents);
  pkifilecontents = nullptr;

  // 4. Add profile to .profiles
  Profiles::Add(profile_name, path);
}
} // namespace gpki
