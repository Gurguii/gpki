#include <iostream>
#include "globals.hpp"
#include "database/profiles.cpp"
#include "database/entities.cpp"
#include "pki/entities.cpp"
#include "pki/tls.cpp"
#include "profiles/profiles.cpp"
#include "entities/entities.cpp"
#include "parser.cpp"
#include "help/usage.cpp"

int main(int argc, const char **args)
{
  if(parse(argc-1,args+1)){
    // something went wrong during parsing
    std::cout << gpki::globals::lasterror << "\n";
    // usage();
    return -1;
  }

  // Call desired action
  gpki::globals::action_ptr();
  
  return 0;
}
