#include "entities.hpp"

using namespace gpki;

int entities::add() {
  std::string input;

  // Set country name
  std::cout << "Country Name (2 letter code) [" << SUBJECT.country << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    SUBJECT.country = input;
  }
  // Set state name
  std::cout << "State or Province Name (full name) [" << SUBJECT.state << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    SUBJECT.state = input;
  }
  // Set location
  std::cout << "Locality Name [" << SUBJECT.location << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    SUBJECT.location = input;
  }
  // Set organisation
  std::cout << "Organisation Name [" << SUBJECT.organisation << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    SUBJECT.organisation = input;
  }
  // *MANDATORY Set common name
  input.assign("");
  std::cout << "Common Name: ";
  std::getline(std::cin, input);
  while (input.empty()) {
    std::cout << "please introduce a common name: ";
    std::getline(std::cin, input);
  };
  SUBJECT.cn = input;

  // Set email
  std::cout << "Email Address: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    SUBJECT.email = input;
  }
  // set files paths
  ENTITY.csr_path = PROFILE.source_dir + SLASH + "pki" + SLASH + "reqs" +
                    SLASH + ENTITY.subject.cn + "-csr." + OUT_FORMAT;
  ENTITY.key_path = PROFILE.source_dir + SLASH + "pki" + SLASH + "keys" +
                    SLASH + ENTITY.subject.cn + "-key." + OUT_FORMAT;
  ENTITY.cert_path = PROFILE.source_dir + SLASH + "pki" + SLASH + "certs" +
                     SLASH + ENTITY.subject.cn + "-crt." + OUT_FORMAT;
  // set profile id
  ENTITY.profile_id = PROFILE.id;
  // set entity serial
  ENTITY.serial = gpki::pki::update_serial();

  // run openssl command
  std::string command = X509 == X509_CA ? get_openssl_CA_command() : get_openssl_command(); 
  if(system(command.c_str())){
    seterror("openssl command failed\n");
    return -1;
  }

  // insert entity to database
  if(database::entities::insert(ENTITY)){
    seterror("couldn't insert entity into database\n");
    return -1;
  };

  return 0;
}
using namespace globals::pki;
std::string entities::get_openssl_command() {
  std::string command =
      "openssl req -newkey " + KEY_FORMAT + ":" + std::to_string(KEY_SIZE) +
      " -x509 -out " + ENTITY.cert_path + " -keyout " + ENTITY.key_path +
      " -config " + globals::metainfo::config + SLASH + "x509" + SLASH +
      (X509 == X509_CLIENT ? "client" : "server") + " -CA " +
      PROFILE.source_dir + SLASH + "pki" + SLASH + "ca" + SLASH +
      "ca-crt.pem " + "-CAkey " + PROFILE.source_dir + SLASH + "pki" + SLASH +
      "ca" + SLASH + "ca-key.pem -noenc -subj " + ENTITY.subj_oneliner() +
      " -days " + std::to_string(DAYS) + "  -set_serial " + ENTITY.serial +
      " -inform " + IN_FORMAT + " -outform " + OUT_FORMAT;
  return command;
}
std::string entities::get_openssl_CA_command() {
  std::string command =
      "openssl req -newkey " + key_format + ":" + std::to_string(key_size) +
      " -x509 -out " + PROFILE.source_dir + SLASH + "pki" + SLASH + "ca" +
      SLASH + "ca-crt.pem" + " -keyout " + PROFILE.source_dir + SLASH + "pki" +
      SLASH + "ca" + SLASH +
      "ca-key.pem"
      " -config " +
      globals::metainfo::config + SLASH + "x509" + SLASH + "ca -noenc -subj " +
      ENTITY.subj_oneliner() += " -inform " + IN_FORMAT + " -outform " + OUT_FORMAT;
  return command;
}
