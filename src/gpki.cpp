#include "globals.cpp"
#include "parser.cpp"
#include "pki/build-ca.cpp"
#include "pki/build-client.cpp"
#include "pki/build-server.cpp"
#include "pki/init-pki.cpp"
#include "profiles/sqlite3_db.cpp"
#include <iostream>
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
