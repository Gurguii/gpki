#include "sqlite3_db.hpp"

namespace gpki {

std::string __db_create_statement = R"(
CREATE TABLE profiles (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  profile_name TEXT NOT NULL,
  source_dir TEXT,
  certs TEXT,
  keys TEXT,
  ca TEXT,
  reqs TEXT,
  serial TEXT,
  x509 TEXT,
  templates TEXT,
  openssl_config TEXT,
  logs TEXT,
  database TEXT,
  crl TEXT,
  packs TEXT
))";

#define CREATE_STATEMENT __db_create_statement.c_str()

std::string __db_insert_template =
    "INSERT INTO profiles (profile_name, source_dir, certs, keys, ca, reqs, "
    "serial, x509, templates, openssl_config, logs, database, crl, packs)"
    "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
    "'%s', '%s', '%s')";
#define INSERT_TEMPLATE __db_insert_template.c_str()

std::vector<const char *> pki_structure_relative_directory_paths{
    "templates", "packs",        "pki/ca",     "pki/crl",  "pki/certs",
    "pki/keys",  "pki/database", "pki/serial", "pki/reqs", "logs"};

const auto print_results_callback = [](void *ptr, int ncols, char **colvalues,
                                       char **colheaders) {
  printf("== ID: %s ==\n", *colvalues);
  for (int i = 1; i < ncols; ++i) {
    printf("%s: %s\n", *(colheaders + i), *(colvalues + i));
  }
  return 0;
};

const auto populate_ProfileInfo_callback =
    [](void *ptr, int ncols, char **colvalues, char **colheaders) {
      ProfileInfo *_ptr = (ProfileInfo *)ptr;
      // First value is an autoincrement INTEGER 'id'
      _ptr->id = strtol(*colvalues, nullptr, 10);
      _ptr->source_dir = *(colvalues + 2);
      _ptr->certs = *(colvalues + 3);
      _ptr->keys = *(colvalues + 4);
      _ptr->ca = *(colvalues + 5);
      _ptr->reqs = *(colvalues + 6);
      _ptr->serial = *(colvalues + 7);
      _ptr->x509 = *(colvalues + 8);
      _ptr->templates = *(colvalues + 9);
      _ptr->openssl_config = *(colvalues + 10);
      _ptr->logs = *(colvalues + 11);
      _ptr->database = *(colvalues + 12);
      _ptr->crl = *(colvalues + 13);
      _ptr->packs = *(colvalues + 14);
      return 0;
    };

const auto profile_exists_callback = [](void *ptr, int ncols, char **colvalues,
                                        char **colheaders) {
  // ptr is an integer of default value -1, which will be the returned value
  // indicating an error unless this callback gets called, what means profile
  // exists so we set the ptr value to 0, indicating no error
  *((int *)ptr) = 1;
  return 0;
};

const std::string_view &db::get_error() { return lasterror; }

// class 'db'
// Static properties initialization
const char *db::db_path = nullptr;
sqlite3 *db::_db = nullptr;

int db::open_db() {
  if (sqlite3_open(db_path, &db::_db) == SQLITE_OK) {
    return 0;
  }
  lasterror = "couldn't open database\n";
  return -1;
}
int db::close_db() {
  if (sqlite3_close(db::_db) == SQLITE_OK) {
    return 0;
  }
  lasterror = "couldn't close database\n";
  return -1;
}

// Public member methods
int db::initialize(const char *dbpath) {
  db_path = dbpath;
  if (open_db()) {
    return -1;
  }
  // Create or open database
  sqlite3_exec(db::_db, CREATE_STATEMENT, nullptr, nullptr, nullptr);
  return sqlite3_close(db::_db) ? -1 : 0;
}

int db::create_files(ProfileInfo *pinfo, std::string_view dst_config_dir) {
  //
  if (!std::filesystem::create_directories(pinfo->source_dir)) {
    lasterror = "Couldn't create PKI root dir";
    return -1;
  }
  for (const char *&st : pki_structure_relative_directory_paths) {
    if (!std::filesystem::create_directories(pinfo->source_dir + SLASH +
                                             std::string(st))) {
      lasterror = "Couldn't create directory";
      // Remove the whole directory structure
      std::filesystem::remove_all(pinfo->source_dir);
      // TODO - add cleanup function to delete done work if it fails
      return -1;
    };
  }
  // Create crlnumber,serial,index.txt files
  std::ofstream(pinfo->crl + SLASH + "crlnumber")
      .write(DEFAULT_CRLNUMBER, strlen(DEFAULT_CRLNUMBER));
  std::ofstream(pinfo->serial + SLASH + "serial")
      .write(DEFAULT_SERIAL, strlen(DEFAULT_SERIAL));
  std::ofstream(pinfo->database + SLASH + "index.txt", std::ios::app);

  // Copy config directory
  std::filesystem::copy(Globals::config_dir, dst_config_dir,
                        std::filesystem::copy_options::recursive);
  return 0;
};
int db::insert_profile(ProfileInfo &pinfo, std::string_view dst_config_dir) {
  char sql[1024];
  if (snprintf(sql, sizeof(sql), INSERT_TEMPLATE, pinfo.name.c_str(),
               pinfo.source_dir.c_str(), pinfo.certs.c_str(),
               pinfo.keys.c_str(), pinfo.ca.c_str(), pinfo.reqs.c_str(),
               pinfo.serial.c_str(), pinfo.x509.c_str(),
               pinfo.templates.c_str(), pinfo.openssl_config.c_str(),
               pinfo.logs.c_str(), pinfo.database.c_str(), pinfo.crl.c_str(),
               pinfo.packs.c_str()) <= 0) {
    lasterror =
        "in file 'sqlite3_facilities.cpp' line 71 -> snprintf() failed\n";
  };
  if (open_db()) {
    return -1;
  }
  // printf("insert statement -> %s\n", sql);
  if (sqlite3_exec(db::_db, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
    printf("insert_query failed -> %s\n", sql);
    return -1;
  };
  if (create_files(&pinfo, dst_config_dir)) {
    return -1;
  };
  return close_db() ? -1 : 0;
}

int db::select_profile(std::string_view profile_name) {
  if (open_db()) {
    return -1;
  }
  if (!profile_exists(profile_name)) {
    return -1;
  }
  std::string sql = "SELECT * FROM profiles WHERE profile_name = '" +
                    std::string(profile_name) + "'";
  sqlite3_exec(_db, sql.c_str(), print_results_callback, nullptr, nullptr);
  return 0;
}

int db::select_profile() { return select_profile(Globals::profile.name); }

int db::delete_profile(std::string_view profile_name) {
  ProfileInfo pinfo;
  populate_ProfileInfo(profile_name, pinfo);
  std::string sql = "DELETE FROM profiles WHERE profile_name = '" +
                    std::string(profile_name) + "'";
  if (open_db()) {
    return -1;
  }
  if (sqlite3_exec(db::_db, sql.c_str(), nullptr, nullptr, nullptr) !=
      SQLITE_OK) {
    lasterror = "delete query failed";
    return -1;
  };
  if (std::filesystem::remove_all(pinfo.source_dir) == -1) {
    lasterror = "couldn't remove profile source path";
    return -1;
  };
  if (close_db()) {
    return -1;
  }
  return 0;
}
int db::delete_profile() { return delete_profile(Globals::profile.name); }
int db::display_profiles() {
  if (open_db()) {
    return -1;
  }
  sqlite3_exec(db::_db, "SELECT * FROM profiles", print_results_callback,
               nullptr, nullptr);
  return close_db() ? -1 : 0;
}

int db::update_database(std::string_view profile_name,
                        std::unordered_map<std::string, std::string> values) {
  if (open_db()) {
    return -1;
  }
  std::string sql = "UPDATE profiles SET ";
  for (const auto &p : values) {
    sql += (p.first + " = '" + p.second + "' , ");
  }
  // delete last " , "
  sql.erase(sql.end() - 3, sql.end());
  sql += " WHERE profile_name = '" + std::string(profile_name) + "'";
  printf("update command -> %s\n", sql.c_str());
  sqlite3_exec(db::_db, sql.c_str(), nullptr, nullptr, nullptr);
  return close_db() ? -1 : 0;
}
int db::populate_CertCreationCommands(ProfileInfo *ptr,
                                      std::string_view profile_name,
                                      CertCreationCommands &buff) {
  // Populate the command members
  std::string serial{10, '\x00'};
  std::ifstream(ptr->serial + SLASH + "serial").read(&serial[0], serial.size());

  std::string csrpath = ptr->reqs + SLASH + serial + "-csr.pem";
  std::string crtpath = ptr->certs + SLASH + serial + "-crt.pem";
  std::string keypath = ptr->keys + SLASH + serial + "-key.pem";

  std::string csr_command = "openssl req -config " + ptr->openssl_config +
                            " -newkey " + std::to_string(Globals::keysize) +
                            ":" + Globals::keyalgorithm + " -outform " +
                            Globals::outformat + " -out " + csrpath +
                            " -keyout " + keypath + " -noenc";

  std::string crt_command =
      "openssl ca -config " + ptr->openssl_config + " -in " + csrpath +
      " -out " + crtpath + " -extfile " +
      (Globals::x509_extension == X509_CLIENT ? ptr->x509 + SLASH + "client "
                                              : ptr->x509 + SLASH + "server ") +
      (Globals::prompt ? "\0" : "--notext -batch");

  buff.crt_command = std::move(crt_command);
  buff.csr_command = std::move(csr_command);
  return 0;
};

int db::populate_ProfileInfo(std::string_view profile_name, ProfileInfo &buff) {
  if (open_db()) {
    return -1;
  }
  if (buff.name.empty()) {
    buff.name = profile_name;
  }
  std::string sql = "SELECT * FROM profiles WHERE profile_name = '" +
                    std::string(profile_name) + "'";
  sqlite3_exec(db::_db, sql.c_str(), populate_ProfileInfo_callback, &buff,
               nullptr);
  if (buff.name.empty()) {
    return -1;
  }
  return close_db() ? -1 : 0;
}
int db::profile_exists(std::string_view profile_name) {
  int code = 0;
  if (open_db()) {
    return 0;
  }
  std::string sql = "SELECT * FROM profiles WHERE profile_name = '" +
                    std::string(profile_name) + "'";
  sqlite3_exec(db::_db, sql.c_str(), profile_exists_callback, &code, nullptr);
  if (close_db()) {
    return 0;
  }
  return code;
}
int db::create_dhparam(std::string_view outpath) {
  std::string command = "openssl dhparam -out " + std::string(outpath) + " " +
                        std::to_string(Globals::dhparam_keysize);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
int db::create_openvpn_static_key(std::string_view outpath) {
  std::string command = "openvpn --genkey tls-crypt > " + std::string(outpath);
  if (system(command.c_str())) {
    return -1;
  }
  return 0;
}
} // namespace gpki

/*
#include <stdio.h>
int main()
{
  gpki::db::initialize("test.db");
  ProfileInfo info;
  memcpy(info.name,"testprofile",11);
  memcpy(info.keys,"keypath",7);
  memcpy(info.certs,"certs",5);
  memcpy(info.reqs,"reqs",4);
  //gpki::db::insert_profile(info);
  gpki::db::update_database("testprofile",
{{"certs","/home/gurgui/certs"},{"keys","/home/gurgi/keys"},{"reqs","/home/gurgui/reqs"}});
  CertCreationInfo cinfo;
  gpki::db::populate_CertCreationInfo("testprofile",cinfo);
  printf("cert -> %s\nkey -> %s\nreq -> %s\n", cinfo.crt, cinfo.key, cinfo.csr);
  gpki::db::populate_ProfileInfo("testprofile",info);
  printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n",info.openssl_config,info.certs,info.name,info.reqs,info.keys,info.ca,info.logs);
  if(gpki::db::profile_exists("testprofile")){
    printf("testprofile exists\n");
  }
  if(gpki::db::profile_exists("wiski")){
    printf("wiski exists\n");
  }
  return 0;
}
*/
