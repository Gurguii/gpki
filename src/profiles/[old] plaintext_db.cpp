#include "[old] plaintext_db.hpp"
namespace gpki {
// Loads variables declared in the current profile's .pkiconf into
// _profiles
int PlaintextDB::Initialize() {
  sqlite3* db;
  sqlite3_open(Globals::profiles_db.data(), &db);
  if(db == nullptr){
    return -1;
  }

  _path = Globals::profiles_db;
  _filesize = std::filesystem::file_size(_path);

  std::ifstream file(_path.data());
  if (!file.is_open()) {
    return -1;
  }
  std::string line = "";
  std::string_view key = "";
  while (getline(file, line)) {
    if (line[0] == '#') {
      // its a comment
      continue;
    }

    if (!line.find('=')) {
      std::cout << "[warning] omitting line " << line << " doesn't have '='\n";
      continue;
    };

    char *key = nullptr;
    char *value = nullptr;
    #ifdef _WIN32
    key = strtok_s((char *)line.c_str(), "=", &value);
    #else
key = strtok_r((char*)line.c_str(), "=", &value);
  #endif
    if (key == nullptr) {
      std::cerr << "null key\n";
      return -1;
    }
    #ifdef _WIN32
    value = strtok_s(nullptr, "=", &value);
    #else
value = strtok_r(nullptr, "=", &value);
    #endif
    if (value == nullptr) {
      std::cerr << "null value\n";
      return -1;
    }
    _profiles.insert(std::pair<std::string, std::string>{key, value});
  }
  return 0;
}

int PlaintextDB::Find(std::string_view key) {
  return (_profiles.find(key.data()) == _profiles.end()) ? 0 : 1;
}

int PlaintextDB::Total() { return _profiles.size(); }

int PlaintextDB::Add(std::string_view profile_name, std::string_view base_dir) {
  if (Find(profile_name)) {
    // profile found
    std::cout << "profile '" << profile_name << "' already exists\n";
    return -1;
  }
  // add entry to _profiles std::map
  _profiles[profile_name.data()] = base_dir;
  std::cout << "[info] Adding entry for '" << profile_name << "'\n";
  // add entry to .profiles file
  std::ofstream profiles_db(_path.data(), std::ios::binary | std::ios::app);
  profiles_db << profile_name << "=" << base_dir << "\n";
  profiles_db.close();
  return 0;
}

int PlaintextDB::Remove(std::string_view profile_name) {
  std::ifstream profiles_db(_path.data());
  std::ofstream tmp_profile_file(".tmp.profiles");
  std::string line;
  while (getline(profiles_db, line)) {
    if (line[0] == '#' || !line.find('=')) {
      // its a comment or does not contain delimiter =
      continue;
    }

    char *path = nullptr;
    char *name = nullptr;
    #ifdef _WIN32
    path = strtok_s(&line[0], "=", &name);
    #else
name = strtok_r(&line[0], "=", &path);
    #endif
    #ifdef _WIN32
    name = strtok_s(nullptr, "=", &name);
    #else
path = strtok_r(nullptr, "=", &path);
    #endif
    if (strcmp(name, profile_name.data())) {
      // not the profile, write the line to tmpfile
      tmp_profile_file << path << "=" << name << "\n";
    }else{
      if(std::filesystem::remove_all(path) == -1){
        Globals::lasterror = "Couldn't recursively remove '" + std::string(path) + "'\n";
      };
    }
  }
  std::filesystem::rename(".tmp.profiles", _path);
  return 0;
}

/* Populates the profileInfo structure with info about profile with name
 * profile_name returns -1 on error 0 on success */
int PlaintextDB::Get(std::string_view profile_name, profileInfo &pinfo) {
  /* Variable delimiter is = */
  std::string pkiconf_path =
      _profiles[profile_name.data()] + SLASH + "config" + SLASH + ".pkiconf";

  if (!std::filesystem::exists(pkiconf_path)) {
    Globals::lasterror = "file '" + std::string(pkiconf_path) + "' doesn't exist\n";
    return -1;
  }

  std::ifstream pkiconf_contents(pkiconf_path);
  if (!pkiconf_contents.is_open()) {
    Globals::lasterror = "couldn't open file '" + std::string(pkiconf_path) + "'\n";
    return -1;
  }

  std::string line{};
  while (getline(pkiconf_contents, line)) {
    //printf("line: %s\n", line.c_str());
    //fflush(0);
    if (line[0] == '#' || !line.find("=") || line.size() == 0) {
      continue;
    }
    char *name = nullptr;
    char *path = nullptr;

    #ifdef _WIN32
    name = strtok_s(line.data(), "=", &path);
    #else
name = strtok_r(line.data(), "=", &path);
    #endif
    if (name == nullptr) {
      // this shouldn't be reached, but in case
      // not a valid delimiter
      continue;
    }
    #ifdef _WIN32
    path = strtok_s(nullptr, "=", &path);
    #else
path = strtok_r(nullptr, "=", &path);
    #endif

    if (!strcmp(name, "certs")) {
      pinfo.certs = path;
    } else if (!strcmp(name, "keys")) {
      pinfo.keys = path;
    } else if (!strcmp(name, "ca")) {
      pinfo.ca = path;
    } else if (!strcmp(name, "x509_dir")) {
      pinfo.x509 = path;
    } else if (!strcmp(name, "serial")) {
      pinfo.serial = path;
    } else if (!strcmp(name, "templates")) {
      pinfo.templates = path;
    } else if (!strcmp(name, "openssl_config_file")) {
      pinfo.openssl_config = path;
    } else if (!strcmp(name, "logs")) {
      pinfo.logs = path;
    } else if (!strcmp(name, "reqs")) {
      pinfo.reqs = path;
    }
  }
  return 0;
};
int PlaintextDB::List(){
  std::ifstream file(Globals::profiles_db);
  std::string line;
  while(std::getline(file,line)){
    if(line[0] == '#' || !line.find('='))
    {continue;}
    char *profile_name;
    char *source_path;
    #ifdef _WIN32
    profile_name = strtok_s(&line[0], "=", &source_path);
    source_path = strtok_s(&line[0], "=", &source_path); 
    #else
    profile_name = strtok_r(&line[0], "=", &source_path);
    source_path = strtok_r(nullptr, "=", &source_path);
    #endif
    printf("== %s ==\nsource directory -> %s\n", profile_name, source_path);
  }
  return 0;
    }
int PlaintextDB::ShowInfo(std::string_view profile_name){
  profileInfo pinfo;
  if(PlaintextDB::Get(profile_name, pinfo)){
    return -1;
  };
  /* Print formatted output */
  printf("== %s ==\nopenssl.conf: %s\n", profile_name.data(), pinfo.openssl_config);
  return 0;
}
} // namespace gpki
