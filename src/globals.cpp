#include "globals.hpp"

/* Initialized with Globals::Initialize() */
std::string Globals::base_dir;
std::string Globals::config_dir;
std::string Globals::profiles_db;

/* Initialized by parser() */
std::vector<std::string> Globals::subopts;

/* Behaviour variables */
// uint8_t Globals::verbose = 0;
// uint8_t Globals::prompt = 1;

/* PKI variables */
// int Globals::keysize = 2048;
// std::string Globals::keyalgorithm = "rsa";
// std::string Globals::outformat = "PEM";
// GPKI_X509 Globals::x509_extension = X509_NONE;

/* Last error message */
// std::string Globals::lasterror = "";
// GPKI_ACTION Globals::action = ACTION_NONE;

void Globals::Initialize() {
  Globals::base_dir = CURRENT_PATH;
  Globals::profiles_db = base_dir + SLASH + ".gpki.db";
  Globals::config_dir = base_dir + SLASH + ".." + SLASH + "config";
}
int Globals::GetSubjectInfo() {
  std::string input;

  // Set country name
  std::cout << "Country Name (2 letter code) [" << Globals::subject.country
            << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::subject.country = input;
  }
  // Set state name
  std::cout << "State or Province Name (full name) [" << Globals::subject.state
            << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::subject.state = input;
  }
  // Set location
  std::cout << "Locality Name [" << Globals::subject.location << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::subject.location = input;
  }
  // Set organisation
  std::cout << "Organisation Name [" << Globals::subject.organisation << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::subject.organisation = input;
  }
  // *MANDATORY Set common name
  input.assign("");
  std::cout << "Common Name: ";
  std::getline(std::cin, input);
  while (input.empty()) {
    std::cout << "please introduce a common name: ";
    std::getline(std::cin, input);
  };
  Globals::subject.cn = input;

  // Set email
  std::cout << "Email Address: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::subject.email = input;
  }
  snprintf(&Globals::subject_oneliner[0], Globals::subject_oneliner.size(),
           SUBJECT_TEMPLATE, Globals::subject.country.c_str(),
           Globals::subject.state.c_str(), Globals::subject.location.data(),
           Globals::subject.organisation.c_str(), Globals::subject.cn.data(),
           Globals::subject.email.c_str());
  return 0;
}
