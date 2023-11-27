#include <string>
#include <iostream>

void usage()
{
  std::string st = R"(
Author: Airán 'Gurguii' Gómez
Description: gpki helps creating/managing pki's mainly focused for VPN use

Usage: gpki.sh <action> <options>                                                        
                                                                                         
[ ACTIONS ]                                                                              
init-pki | build-ca | build-server | build-client | remove-profile | remove-all-profiles 
                                                                                         
[ OPTIONS ]                                                                              
not added yet                                                                            
                                                                                         
- Create new profile -                                                                   
./gpki.sh init-pki /home/gurgui/[profile]                                                
                                                                                         
- Create new entities -                                                                  
./gpki.sh build-ca     [profile] [cn]                                                    
./gpki.sh build-server [profile] [cn]                                                    
./gpki.sh build-client [profile] [cn]                                                    
                                                                                         
- Create packs -                                                                           
NOT IMPLEMENTED ./gpki.sh create-pack  [profile] [cn] [options]                          
                                                                                         
- Removing profiles -                                                                    
./gpki.sh remove-profile [profile]                                                       
./gpki.sh remove-all-profiles                                                            
)";
  std::cout << st << "\n";
}
