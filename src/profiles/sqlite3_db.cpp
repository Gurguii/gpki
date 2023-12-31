#include "sqlite3_db.hpp"

namespace gpki {

std::string __create_profiles_table_template = R"(
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
#define CREATE_PROFILES_STATEMENT __create_profiles_table_template.c_str()

std::string __create_certificates_table_template = R"(
CREATE TABLE certificates (
 id INTEGER PRIMARY KEY AUTOINCREMENT,
 cn VARCHAR(255) NOT NULL,
 serial VARCHAR(255) NOT NULL,
 cert VARCHAR(255) NOT NULL,
 key VARCHAR(255) NOT NULL,
 req VARCHAR(255) NOT NULL,
 profile_id INTEGER NOT NULL,
 FOREIGN KEY(profile_id) REFERENCES profiles(id)
)
)";
#define CREATE_CERTIFICATES_STATEMENT                                          \
  __create_certificates_table_template.c_str()

const char *__db_insert_profile_template =
    "INSERT INTO profiles (profile_name, source_dir, certs, keys, ca, reqs, "
    "serial, x509, templates, openssl_config, logs, database, crl, packs)"
    "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
    "'%s', '%s', '%s')";
#define INSERT_PROFILE_TEMPLATE __db_insert_profile_template

const char *__db_insert_certificate_template =
    "INSERT INTO entities (cn,serial,cert,key,req,profile_id) VALUES "
    "('%s','%s','%s','%s','%s','%s')";
#define INSERT_ENTITY_TEMPLATE __db_insert_certificate_template

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

const auto populate_EntityInfo_callback =
    [](void *ptr, int ncols, char **colvalues, char **colheaders) {
      // this function will receive a pointer to a struct EntityInfo
      // so if it gets executed it will populate the EntityInfo with the
      // extracted info
      auto *_ptr = (EntityInfo *)ptr;
      _ptr->subject.cn = *(colvalues + 1);
      _ptr->serial = strtol(*(colvalues + 2), nullptr, 10);
      _ptr->cert_path = *(colvalues + 3);
      _ptr->key_path = *(colvalues + 4);
      _ptr->csr_path = *(colvalues + 5);
      _ptr->profile_id = strtol(*(colvalues + 6), nullptr, 10);
    };
const std::string &db::get_error() { return lasterror; }

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
  // Create profiles table
  sqlite3_exec(db::_db, CREATE_PROFILES_STATEMENT, nullptr, nullptr, nullptr);
  // Create certificates table
  sqlite3_exec(db::_db, CREATE_CERTIFICATES_STATEMENT, nullptr, nullptr,
               nullptr);
  return close_db();
}

int db::create_pki_directory_structure(ProfileInfo *pinfo) {
  //
  if (!std::filesystem::create_directories(pinfo->source_dir)) {
    lasterror = "Couldn't create PKI root dir";
    return -1;
  }
  for (auto pair : Globals::pki_relative_directory_paths) {
    // pair.first -> uint8_t referencing path on enum class _pki_relative_paths
    // pair.second -> std::string with the relative path
    if (!std::filesystem::create_directories(pinfo->source_dir + SLASH +
                                             pair.second)) {
      lasterror = "Couldn't create directory " + pair.second;
      // Remove the whole profile structure
      std::filesystem::remove_all(pinfo->source_dir);
      return -1;
    }
  }
  // Create crlnumber,serial,index.txt files
  std::ofstream(pinfo->crl + SLASH + "crlnumber")
      .write(DEFAULT_CRLNUMBER, strlen(DEFAULT_CRLNUMBER));
  std::ofstream(pinfo->serial + SLASH + "serial")
      .write(DEFAULT_SERIAL, strlen(DEFAULT_SERIAL));
  std::ofstream(pinfo->database + SLASH + "index.txt", std::ios::app);

  // Copy config directory
  std::filesystem::copy(Globals::config_dir,
                        (pinfo->source_dir + SLASH + "config"),
                        std::filesystem::copy_options::recursive);
  return 0;
};
int db::insert_profile(ProfileInfo &pinfo) {
  char sql[1024];
  if (snprintf(sql, sizeof(sql), INSERT_PROFILE_TEMPLATE,
               Globals::profile.name.c_str(),
               Globals::profile.source_dir.c_str(), pinfo.certs.c_str(),
               Globals::profile.keys.c_str(), pinfo.ca.c_str(),
               pinfo.reqs.c_str(), Globals::profile.serial.c_str(),
               pinfo.x509.c_str(), Globals::profile.templates.c_str(),
               pinfo.openssl_config.c_str(), Globals::profile.logs.c_str(),
               pinfo.database.c_str(), pinfo.crl.c_str(),
               Globals::profile.packs.c_str()) <= 0) {
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

  return close_db() ? -1 : 0;
}
int db::insert_entity(EntityInfo &cinfo) {
  if (open_db()) {
    return -1;
  }
  if (!profile_exists(cinfo.profile_id) || entity_exists(cinfo.subject.cn)) {
    // not insertable
    std::cout << "not insertable -> sqlite3_db.cpp line 201\n";
    return -1;
  }
  char sql[1024]{};
  snprintf(sql, sizeof(sql), INSERT_ENTITY_TEMPLATE, cinfo.subject.cn.c_str(),
           cinfo.serial.c_str(), cinfo.cert_path.c_str(), cinfo.profile_id);
  printf("insert -> %s\n", sql);
  return 0;
};
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
  return close_db();
}
int db::delete_entity(std::string_view cn) {
  std::string sql = "DELETE FROM entities WHERE cn = '" + std::string(cn) + "'";
  if (open_db()) {
    return -1;
  }
  if (sqlite3_exec(db::_db, sql.c_str(), nullptr, nullptr, nullptr) !=
      SQLITE_OK) {
    lasterror = "couldnt' remove entity " + std::string(cn);
    return -1;
  }
  return close_db();
}
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
int db::populate_EntityInfo(ProfileInfo *ptr, EntityInfo &certinfo) {
  // Populate the command members
  std::string serial{10, '\x00'};
  std::ifstream(ptr->serial + SLASH + "serial").read(&serial[0], serial.size());

  std::string csrpath =
      ptr->reqs + SLASH + Globals::certinfo.subject.cn + "-csr.pem";
  std::string crtpath =
      ptr->certs + SLASH + Globals::certinfo.subject.cn + "-crt.pem";
  std::string keypath =
      ptr->keys + SLASH + Globals::certinfo.subject.cn + "-key.pem";

  std::string csr_command =
      "openssl req -config " + ptr->openssl_config + " -newkey " +
      std::to_string(Globals::keysize) + ":" + Globals::keyalgorithm +
      " -outform " + Globals::outformat + " -out " + csrpath + " -keyout " +
      keypath + " -subj '" + Globals::subject_oneliner.c_str() + "' -noenc";

  std::string crt_command =
      "openssl ca -config " + ptr->openssl_config + " -in " + csrpath +
      " -out " + crtpath + " -extfile " +
      (Globals::x509_extension == X509_CLIENT ? ptr->x509 + SLASH + "client "
                                              : ptr->x509 + SLASH + "server ") +
      (Globals::prompt ? "\0" : "--notext -batch");

  certinfo.serial = std::move(serial);
  certinfo.commands.crt_command = std::move(crt_command);
  certinfo.commands.csr_command = std::move(csr_command);
  certinfo.cert_path = std::move(crtpath);
  certinfo.key_path = std::move(keypath);
  certinfo.csr_path = std::move(csrpath);
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
    return -1;
  }
  return code;
}
int db::profile_exists(int profile_id) {
  int code = 0;
  if (open_db()) {
    return -1;
  }
  std::string sql =
      "SELECT * FROM profiles WHERE id = " + std::to_string(profile_id);
  sqlite3_exec(db::_db, sql.c_str(), profile_exists_callback, &code, nullptr);
  if (close_db()) {
    return -1;
  }
  return code;
}

int db::entity_exists(std::string &cn) {
  int code = 0;
  if (open_db()) {
    return -1;
  }
  std::string sql = "SELECT * FROM certificates WHERE cn = '" + cn + +"'";
  sqlite3_exec(db::_db, sql.c_str(), profile_exists_callback, &code, nullptr);
  if (close_db()) {
    return -1;
  }
  return code;
}
int db::entity_exists(int certificate_id) {
  int code = 0;
  if (open_db()) {
    return -1;
  }
  std::string sql = "SELECT * FROM certificates WHERE id = '" +
                    std::to_string(certificate_id) + +"'";
  sqlite3_exec(db::_db, sql.c_str(), profile_exists_callback, &code, nullptr);
  if (close_db()) {
    return -1;
  }
  return code;
}
} // namespace gpki
