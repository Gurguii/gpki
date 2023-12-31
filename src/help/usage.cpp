#include <iostream>
#include <string>

void usage() {
  std::string st = R"(
Author: Airán 'Gurguii' Gómez
Description: gpki helps creating/managing pki's mainly focused for VPN use

Usage: gpki <action>* <profile> <options>
note: action is mandatory, not all actions require a profile name, e.g init-pki

[ ACTIONS ]                                                                              
init-pki | build-ca | build-server | build-client | remove-profile | remove-all-profiles 
profiles-list | profile-info | profile-remove 

- Create new profile -                                                                   
./gpki init-pki                                                
                                                                                         
- Create new certificates -                                                                  
./gpki build-ca     [profile]                                                    
./gpki build-server [profile]                                                    
./gpki build-client [profile]                                                    

- Revoke certificates -
./gpki revoke-certificate [profile] [cn]

- Generate crl -
./gpki generate-crl [profile]

- Print profile info -
./gpki profiles-list 
./gpki profile-info [profile]

- Remove profiles -
./gpki profile-remove [profile]

- Create packs -                                                                           
NOT IMPLEMENTED ./gpki create-pack  [profile] [cn] [options]                          
                                                                                         
- Removing profiles -                                                                    
./gpki profile-remove [profile]                                                       
NOT IMPLEMENTED ./gpki remove-all-profiles                                                            
)";
  std::cout << st << "\n";
}
