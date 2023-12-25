#ifdef GPKI_USE_SQLITE
#include "profiles/sqlite3_db.cpp"
#else
#include "profiles/plaintext_db.cpp"
#endif
#include "globals.cpp"
#include "parser.cpp"
#include "pki/build-ca.cpp"
#include "pki/build-client.cpp"
#include "pki/build-server.cpp"
#include "pki/init-pki.cpp"
#include "pki/utils.cpp"
#include "profiles/profiles.cpp"
#include <iostream>
using namespace gpki;

int main(int argc, const char **args) {
  Globals::Initialize();

  if (Parse(argc, args)) {
    // something went wrong
    // usage();
    return -1;
  };

  /* Check action and call appropiate method */
  switch (static_cast<uint8_t>(Globals::action)) {
  /* PKI ACTIONS */
  case (ACTION_INIT_PKI):
    pki_init();
    break;
  case (ACTION_BUILD_CA):
    build_ca();
    break;
  case (ACTION_BUILD_SERVER):
    build_server();
    break;
  case (ACTION_BUILD_CLIENT):
    build_client();
    break;
  /* PROFILE ACTIONS */
  case (ACTION_PROFILE_LIST):
    Profiles::List();
    break;
  case (ACTION_PROFILE_REMOVE):
    Profiles::Remove(Globals::profile_name);
    break;
  case (ACTION_PROFILE_INFO):
    Profiles::ShowInfo(Globals::profile_name);
    break;
  case (ACTION_NONE):
    // no action to be done
    std::cout << "ACTION = NONE\n";
    break;
  default:
    // should never happen
    std::cerr << "[error] no proper action given\n";
    return -1;
  };
  
  if(!Globals::lasterror.empty()){
    std::cout << Globals::lasterror << "\n";
  }
  return 0;
}
