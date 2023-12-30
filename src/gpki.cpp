#include "globals.cpp"
#include "parser.cpp"
#include "pki/build_certificates.cpp"
#include "pki/init-pki.cpp"
#include "pki/security_key_creation.cpp"
#include "profiles/sqlite3_db.cpp"
using namespace gpki;

int main(int argc, const char **args) {
  Globals::Initialize();

  if (Parse(argc, args)) {
    // something went wrong
    // usage();
    return -1;
  };

  /* Call desired action (method) */
  Globals::action();

  if (Globals::lasterror != "no errors yet") {
    std::cout << Globals::lasterror << "\n";
  }

  // No errors through the whole execution
  return 0;
}
