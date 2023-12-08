#include "../profiles/profiles.hpp"
#include "utils.hpp"
#include <string_view>
namespace gpki {
int build_client() {
  CertCreationInfo info;
  GetCertCreationInfo(Globals::profile_name, info, x509_client);
  if (system(info.csr_command)) {
    return -1;
  }
  return system(info.crt_command);
}
} // namespace gpki
