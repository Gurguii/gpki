#include "utils.hpp"
#include <string_view>
#include "../globals.hpp"
namespace gpki {
int build_client() {
  CertCreationInfo info;
  GetCertCreationInfo(Globals::profile_name, info, x509_client);
  printf("csr -> %s\ncrt -> %s\npaths: %s : %s\n", info.csr_command, info.crt_command, info.csr, info.crt);
  if (system(info.csr_command)) {
    return -1;
  }
  return system(info.crt_command);
}
} // namespace gpki
