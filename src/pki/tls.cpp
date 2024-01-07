#include "tls.hpp"

int gpki::tls::create_dhparam(std::string_view outpath) {
  std::string command = "openssl dhparam -out " + std::string(outpath) + " " +
                        std::to_string(gpki::globals::tls::dhparam_keysize);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
int gpki::tls::create_openvpn_static_key(std::string_view outpath) {
  std::string command = "openvpn --genkey tls-crypt " + std::string(outpath);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
