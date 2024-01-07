#include "entities.hpp"

using namespace gpki;

int pki::build_ca() {
  X509 = X509_CA;
  entities::add();
  return 0;
}
int pki::build_server() {
  X509 = X509_SERVER;
  entities::add();
  return 0;
}
int pki::build_client() {
  X509 = X509_CLIENT;
  entities::add();
  return 0;
}

int pki::get_serial(){
  std::string serial_file =
      PROFILE.source_dir + SLASH + "pki" + SLASH + "serial" + SLASH + "serial";
  std::string s(std::filesystem::file_size(serial_file),'\x00');
  return 0;
}

std::string pki::update_serial() {
  std::string serial_file =
      PROFILE.source_dir + SLASH + "pki" + SLASH + "serial" + SLASH + "serial";
  std::string serial(std::filesystem::file_size(serial_file), '\x00');
  std::ifstream(serial_file).read(&serial[0], serial.size());
  int nserial = strtol(&serial[0], nullptr, 10);
  nserial += 1;
  std::ofstream(serial_file)
      .write(&std::to_string(nserial)[0], std::to_string(nserial).size());
  return serial;
}
