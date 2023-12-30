#include "security_key_creation.hpp"
namespace gpki {
int create_dhparam(std::string_view outpath) {
  std::string command = "openssl dhparam -out " + std::string(outpath) + " " +
                        std::to_string(Globals::dhparam_keysize);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
int create_openvpn_static_key(std::string_view outpath) {
  std::string command = "openvpn --genkey tls-crypt > " + std::string(outpath);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
} // namespace gpki
