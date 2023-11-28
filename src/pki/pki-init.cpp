#include "pki-init.hpp"

namespace gpki
{

std::vector<const char *>pki_structure_relative_directory_paths{
  "x509","templates","packs","pki/ca","pki/crl","pki/certs","pki/keys","pki/database","pki/serial","logs"
};

void pki_init(){
  // ask for profile_name and path making sure path is absolute (starts with '/')
  std::string profile_name = "";
  std::string path = "";
  std::string input = "";
  
  do{
    std::cout << "[+] Please introduce desired profile name: ";
    std::getline(std::cin,input);
  }while(Profiles::Find(input));

  profile_name = input;

  do{
    std::cout << "[+] Please introduce pki base dir (absolute path): ";
    std::getline(std::cin,input);
  }while(input[0] != '/');
  path=std::move(input);
  
  // Create PKI structure

  // 1. create directories
  for(const char *&dir : pki_structure_relative_directory_paths){
    if(!std::filesystem::create_directories(path+"/"+dir)){
      std::cout << "[FAIL] Create directory '" << dir << "' failed\n";
      // TODO - add cleanup function to delete what's been done
      // e.g remove globals::base_dir
    };
  }

  // 2. create files
  std::ofstream(path+"/pki/crl/crlnumber").write("1000\n",5);
  std::ofstream(path+"/pki/serial/serial").write("01\n",3);
  std::ofstream(path+"/pki/database/index.txt");
  char command[120];
  memset(command,0,sizeof(command));
  snprintf(command,sizeof(command),"cp -rf %s %s/config",globals::config_dir.c_str(),path.c_str());
  system(command);
  memset(command,0,sizeof(command));                                                                                                                     
  snprintf(command,sizeof(command),"sed -i 's#GPKI_BASEDIR#%s/pki#' %s",path.c_str(), (path + "/config/gopenssl.cnf").c_str()); 
  system(command);                                                                                                                                        
  memset(command,0,sizeof(command));
} 
} // namespace gpki
