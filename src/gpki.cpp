#include <filesystem>
#include <iostream>
#include <map>

#include "globals.cpp"
#include "parser.cpp"
#include "pki/build-ca.cpp"
#include "pki/build-client.cpp"
#include "pki/build-server.cpp"
#include "pki/pki-init.cpp"
#include "profiles/profiles.cpp"

using namespace gpki;

int main(int argc, const char **args) {
  // if(Parse(argc,args)){
  //   return -1;
  // };
  globals::Initialize();
  Profiles::Initialize();
  pki_init();
  build_ca("test");
  build_server("test");
  // build_ca("tw","OTHERCA");
  return 0;
}
