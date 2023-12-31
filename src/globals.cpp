#include "globals.hpp"

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

namespace gpki {
/* Initialized with Globals::Initialize() */
std::string Globals::base_dir;
std::string Globals::config_dir;
std::string Globals::profiles_db;

/* Initialized by parser() */
std::vector<std::string> Globals::subopts;

void Globals::Initialize() {
  Globals::base_dir = CURRENT_PATH;
  Globals::profiles_db = base_dir + SLASH + ".gpki.db";
  Globals::config_dir = base_dir + SLASH + ".." + SLASH + "config";
}
int Globals::GetCertInfo() {
  std::string input;

  // Set country name
  std::cout << "Country Name (2 letter code) ["
            << Globals::certinfo.subject.country << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::certinfo.subject.country = input;
  }
  // Set state name
  std::cout << "State or Province Name (full name) ["
            << Globals::certinfo.subject.state << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::certinfo.subject.state = input;
  }
  // Set location
  std::cout << "Locality Name [" << Globals::certinfo.subject.location << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::certinfo.subject.location = input;
  }
  // Set organisation
  std::cout << "Organisation Name [" << Globals::certinfo.subject.organisation
            << "]: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::certinfo.subject.organisation = input;
  }
  // *MANDATORY Set common name
  input.assign("");
  std::cout << "Common Name: ";
  std::getline(std::cin, input);
  while (input.empty()) {
    std::cout << "please introduce a common name: ";
    std::getline(std::cin, input);
  };
  Globals::certinfo.subject.cn = input;

  // Set email
  std::cout << "Email Address: ";
  std::getline(std::cin, input);
  if (!input.empty()) {
    Globals::certinfo.subject.email = input;
  }

  snprintf(&Globals::subject_oneliner[0], Globals::subject_oneliner.size(),
           SUBJECT_TEMPLATE, Globals::certinfo.subject.country.c_str(),
           Globals::certinfo.subject.state.c_str(),
           Globals::certinfo.subject.location.data(),
           Globals::certinfo.subject.organisation.c_str(),
           Globals::certinfo.subject.cn.data(),
           Globals::certinfo.subject.email.c_str());
  db::populate_EntityInfo(&Globals::profile, Globals::certinfo);
  return 0;
}
int Globals::InsertProfile() { return db::insert_profile(Globals::profile); }
int Globals::CreateFiles() {
  return db::create_pki_directory_structure(&Globals::profile);
}
int Globals::DeleteProfile() {
  return db::delete_profile(Globals::profile.name);
}
int Globals::SelectProfile() {
  return db::select_profile(Globals::profile.name);
}
int Globals::InsertEntity() { return db::insert_entity(Globals::certinfo); }
} // namespace gpki
