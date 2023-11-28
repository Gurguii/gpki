#include "profiles.hpp"
const auto has = [](std::string_view st, char c){
  for(int i = 0; i < st.size(); ++i){
    if(st[i] == c){
      return 1;
    }
  }
  return 0;
};

namespace gpki
{
int Profiles::Initialize(std::string_view filepath){
  _path = filepath;
  _filesize = std::filesystem::file_size(filepath);
 
  std::ifstream file(filepath.data());
  if(!file.is_open()){
    return -1;
  }
  std::string line = "";
  std::string_view key = "";
  char *value; 
  while(getline(file,line)){
    if(line[0] == '#'){
      // its a comment
      continue; 
    }
    if(!has(line,':')){
      std::cout << "[warning] omitting line " << line << " doesn't have ':'\n";
      continue; 
    };
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
int Profiles::Add(std::string_view profile_name, std::string_view base_dir){
  if(_profiles.empty()){
    // no profiles
    return -1;
  }
  if(Find(profile_name)){
    // profile found
    std::cout << "profile '" << profile_name << "' already exists\n";
    return -1;
  }
  std::ofstream profiles_file(_path.data(),std::ios::binary | std::ios::app);
  profiles_file << profile_name << ":" << base_dir << "\n";
  return 0;
}

int Profiles::Remove(std::string_view profile_name){
  if(_profiles.empty()){
    // profile is empty
    return -1;
  }
  if(!Find(profile_name)){
    // profile not found
    return -1;
  }
  std::ifstream profiles_file(_path.data());
  std::ofstream tmp_profile_file(".tmp.profiles");
  std::string line;
  while(getline(profiles_file,line)){
    if(line[0] == '#' || !has(line,':')){
      // its a comment or does not contain delimiter :
      continue;
    }
    char *path = strtok(&line[0],":");
    char *name = path;
    path = strtok(NULL,":"); 
    if(strcmp(name,profile_name.data())){
      // not the profile, write the line to tmpfile
      tmp_profile_file << name << ":" << path << "\n";
    }
  }
  std::filesystem::remove(_path);
  std::filesystem::rename(".tmp.profiles",_path);
  return 0;
}
} // namespace gpki::profiles
