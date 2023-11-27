#include "profiles.hpp"
int Profiles::Initialize(std::string_view filepath){
  _path = filepath;
  _filesize = std::filesystem::file_size(filepath);
 
  std::ifstream file(filepath.data());
  if(!file.is_open()){
    return -1;
  }
  std::string line;
  std::string key, value;
  while(getline(file,line)){
    value = strtok((char*)line.c_str(), ":");
    key = value;
    value = strtok(NULL,":");
    _profiles.insert(std::pair<std::string,std::string>{key,value});
  }
  return 0;
}
int Profiles::Find(std::string_view key){
  return (_profiles.find(key.data()) == _profiles.end()) ? 0 : 1;  
}
int Profiles::Total(){
  return _profiles.size();
}
int Profiles::Add(std::string_view profile_name){
  
}
int Profiles::Add(std::string_view profile_name, std::string_view base_dir){

}
int Profiles::Remove(std::string_view profile_name){
  if(!Find(profile_name)){
    return -1;
  }
  std::ifstream profiles_file(_path.data());
  std::ofstream tmp_profile_file(".tmp.profiles");
  std::string line;
  while(getline(profiles_file,line)){
    char *name = strtok(&line[0],":");
    if(strcmp(name,profile_name.data())){
      tmp_profile_file << line;
    }
  }
  std::filesystem::remove(_path);
  printf("renaming %s to %s\n",".tmp.profiles",_path.data());
  std::filesystem::rename(".tmp.profiles",_path);
  return 0;
}
