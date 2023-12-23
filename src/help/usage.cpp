#include <string>
#include <iostream>

void usage()
{
  std::string st = R"(
Author: Airán 'Gurguii' Gómez
Description: gpki helps creating/managing pki's mainly focused for VPN use

Usage: gpki <action> <profile> <options>                                                        
                                                                                         
[ ACTIONS ]                                                                              
init-pki | build-ca | build-server | build-client | remove-profile | remove-all-profiles 
                                                                                         
[ OPTIONS ]                                                                              
not added yet                                                                            
                                                                                         
- Create new profile -                                                                   
./gpki init-pki /home/gurgui/[profile]                                                
                                                                                         
- Create new entities -                                                                  
./gpki build-ca     [profile] [cn]                                                    
./gpki build-server [profile] [cn]                                                    
./gpki build-client [profile] [cn]                                                    
                                                                                         
- Create packs -                                                                           
NOT IMPLEMENTED ./gpki create-pack  [profile] [cn] [options]                          
                                                                                         
- Removing profiles -                                                                    
./gpki remove-profile [profile]                                                       
./gpki remove-all-profiles                                                            
)";
  std::cout << st << "\n";
}
