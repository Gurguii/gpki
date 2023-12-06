#include <iostream>
#include <filesystem>
#include <map>

#include "parser.cpp"
#include "globals.cpp"
#include "profiles/profiles.cpp"
#include "pki/build-ca.cpp"
#include "pki/build-server.cpp"
#include "pki/build-client.cpp"
#include "pki/pki-init.cpp"

using namespace gpki;

int main(int argc, const char **args)
{
  //if(Parse(argc,args)){
  //  return -1;
  //};
  globals::Initialize();
  Profiles::Initialize();
  build_ca("tw");
  //build_ca("tw","OTHERCA");
  return 0;
}
