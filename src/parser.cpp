#include <iostream>
#include <cstring>
#include "help/usage.cpp"
/* As reference
 * ./gpki [action] [profile-name] [options] 
 * ./gpki build-ca myvpnserver -cn myCA
 * ./gpki init-pki */

int Parse(int argc, const char **&args)
{
  if(argc == 1){
    // No arguments supplied
    usage();
    return -1;
  }
  // Check if user asking for help
  for(int i = 0; i < argc; ++i){
    const char *arg = *(args+i);
    if(!strcmp(arg,"-h") || strcmp(arg,"--help")){
      usage();
      return -1;
    }
  }
  
  return 0;
}
