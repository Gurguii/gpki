#include "sqlite3_db.hpp"
#include <cstring>


namespace gpki
{

std::string __db_create_statement = R"(
CREATE TABLE profiles (
  profile_name TEXT PRIMARY KEY NOT NULL,
  certs TEXT,
  keys TEXT,
  ca TEXT,
  reqs TEXT,
  serial TEXT,
  x509 TEXT,
  templates TEXT,
  openssl_config TEXT,
  logs TEXT
))";
#define CREATE_STATEMENT __db_create_statement.c_str()

std::string __db_insert_template = R"(INSERT INTO profiles (profile_name, certs, keys, ca, reqs, serial, x509, templates, openssl_config, logs) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s'))";
#define INSERT_TEMPLATE __db_insert_template.c_str()

const auto print_select_statement = [](void *ptr, int ncols, char **colvalues, char **colheaders){
  printf("== %s ==\n",*colvalues);
  for(int i = 1; i < ncols; ++i){
    printf("%s: %s\n", *(colheaders +i), *(colvalues+i));
  }
  return 0;
};

const auto populate_CertCreationInfo_callback = [](void *ptr, int ncols, char **colvalues, char **colheaders){
  for(int i = 0; i < ncols; ++i){
    // printf("i=%i | %s:%s\n", i, *(colheaders+i), *(colvalues+i));
    std::string_view header = *(colheaders +i);
    if(header == "reqs"){
      memcpy(((CertCreationInfo*)ptr)->csr, *(colvalues+i), strlen(*(colvalues+i)) );
    }else if(header == "keys"){
      memcpy(((CertCreationInfo*)ptr)->key, *(colvalues+i), strlen(*(colvalues+i)) );
    }else if(header == "certs"){
      memcpy(((CertCreationInfo*)ptr)->crt, *(colvalues+i), strlen(*(colvalues +i)) );
    }
  }
  return 0;
};

const auto populate_ProfileInfo_callback = [](void *ptr, int ncols, char **colvalues, char **colheaders){
  ProfileInfo *_ptr = (ProfileInfo*)ptr;
  memcpy(_ptr->certs,*(colvalues+1),strlen(*(colvalues+1)));
  memcpy(_ptr->keys,*(colvalues+2),strlen(*(colvalues+2)));
  memcpy(_ptr->ca,*(colvalues+3),strlen(*(colvalues+3)));
  memcpy(_ptr->reqs,*(colvalues+4),strlen(*(colvalues+4)));
  memcpy(_ptr->serial,*(colvalues+5),strlen(*(colvalues+5)));
  memcpy(_ptr->x509,*(colvalues+6),strlen(*(colvalues+6)));
  memcpy(_ptr->templates,*(colvalues+7),strlen(*(colvalues+7)));
  memcpy(_ptr->openssl_config,*(colvalues+8),strlen(*(colvalues+8)));
  memcpy(_ptr->logs,*(colvalues+9),strlen(*(colvalues+9)));
  return 0;
};

const auto find_profile_callback = [](void *ptr, int ncols, char **colvalues, char **colheaders){
  // ptr is an integer of default value -1, which will be the returned value indicating an error unless 
  // this callback gets called, what means profile exists so we set the ptr value to 0, indicating no error
  *((int*)ptr) = 1;
  return 0;
};

const char *&db::get_error(){
  return lasterror;
}

// class 'db'
// Static properties initialization
const char *db::db_path = nullptr;
const char *db::lasterror = nullptr;
sqlite3 *db::_db = nullptr;

int db::open_db(){
  if(sqlite3_open(db_path, &db::_db) == SQLITE_OK){
    return 0;
  }
  lasterror = "couldn't open database\n";
  return -1;
}
int db::close_db(){
  if(sqlite3_close(db::_db) == SQLITE_OK){
    return 0;
  }
  lasterror = "couldn't close database\n";
  return -1;
}

// Public member methods
int db::initialize(const char *dbpath){
    db_path = dbpath;
    if(open_db()){
      return -1;
    }
    // Create or open database
    sqlite3_exec(db::_db, CREATE_STATEMENT, nullptr, nullptr, nullptr);
    return sqlite3_close(db::_db) ? -1 : 0;
  }
int db::insert_profile(ProfileInfo &pinfo){
    char sql[1024];
    if(snprintf(sql,sizeof(sql),INSERT_TEMPLATE,pinfo.name, pinfo.certs, pinfo.keys, pinfo.ca, pinfo.reqs, pinfo.serial, pinfo.x509, pinfo.templates, pinfo.openssl_config, pinfo.logs) <= 0){
      lasterror = "in file 'sqlite3_facilities.cpp' line 71 -> snprintf() failed\n";
    };
    if(open_db()){
      return -1;
    }
    //printf("insert statement -> %s\n", sql);
    sqlite3_exec(db::_db, sql, nullptr, nullptr, nullptr);
    return close_db() ? -1 : 0;
  }
  int db::select_profile(std::string_view profile_name){
    std::string sql = "SELECT * FROM profiles WHERE profile_name = '" + std::string(profile_name) + "'";
    if(open_db()){
      return -1;
    };
    //sqlite3_exec(sqlite3 *, const char *sql, int (*callback)(void *, int, char **, char **), void *, char **errmsg)
    sqlite3_exec(db::_db, sql.c_str(), print_select_statement, nullptr, nullptr);
    return close_db() ? -1 : 0;
  }    
  int db::delete_profile(std::string_view profile_name){
    std::string sql = "DELETE FROM profiles WHERE profile_name = '" + std::string(profile_name) + "'";
    if(open_db()){
      return -1;
    }
    sqlite3_exec(db::_db,sql.c_str(),nullptr,nullptr,nullptr);
    if(close_db()){
      return -1;
    }
    return 0;
  }
  int db::display_profiles(){
    if(open_db()){
      return -1;
    }
    sqlite3_exec(db::_db, "SELECT * FROM profiles", print_select_statement, nullptr, nullptr);
    return close_db() ? -1 : 0;
  }

int db::update_database(std::string_view profile_name, std::unordered_map<std::string,std::string> values){
  if(open_db()){
    return -1;
  }
  std::string sql = "UPDATE profiles SET ";
  for(const auto &p : values){
    sql += (p.first + " = '" + p.second + "' , "); 
  }
  // delete last " , "
  sql.erase(sql.end()-3,sql.end());
  sql += " WHERE profile_name = '" + std::string(profile_name) + "'";
  printf("update command -> %s\n", sql.c_str()); 
  sqlite3_exec(db::_db,sql.c_str(),nullptr,nullptr,nullptr);
  return close_db() ? -1 : 0;
}
int db::populate_CertCreationInfo(std::string_view profile_name, CertCreationInfo &buff){
  if(open_db()){
    return -1;
  }
  std::string sql = "SELECT certs,keys,reqs FROM profiles WHERE profile_name = '" + std::string(profile_name) + "'";
  sqlite3_exec(db::_db, sql.c_str(), populate_CertCreationInfo_callback, &buff,nullptr);
  return close_db() ? -1 : 0;
};
int db::populate_ProfileInfo(std::string_view profile_name, ProfileInfo &buff){
  if(open_db()){
    return -1;
  }
  std::string sql = "SELECT * FROM profiles WHERE profile_name = '" + std::string(profile_name) + "'";
  sqlite3_exec(db::_db, sql.c_str(), populate_ProfileInfo_callback, &buff, nullptr);
  return close_db() ? -1 : 0;
}
int db::profile_exists(std::string_view profile_name){
  int code = 0;
  if(open_db()){
    return -1;
  }
  std::string sql = "SELECT * FROM profiles WHERE profile_name = '" + std::string(profile_name) + "'";
  sqlite3_exec(db::_db,sql.c_str(),find_profile_callback,&code,nullptr);
  if(close_db()){
    return -1;
  }
  return code; 
}
} // namespace gpki

#include <stdio.h>
int main()
{
  gpki::db::initialize("test.db");
  ProfileInfo info;
  /*
  memcpy(info.name,"testprofile",11);
  memcpy(info.keys,"keypath",7);
  memcpy(info.certs,"certs",5);
  memcpy(info.reqs,"reqs",4);
  //gpki::db::insert_profile(info);
  gpki::db::update_database("testprofile", {{"certs","/home/gurgui/certs"},{"keys","/home/gurgi/keys"},{"reqs","/home/gurgui/reqs"}});
  CertCreationInfo cinfo;
  gpki::db::populate_CertCreationInfo("testprofile",cinfo);
  printf("cert -> %s\nkey -> %s\nreq -> %s\n", cinfo.crt, cinfo.key, cinfo.csr);
  gpki::db::populate_ProfileInfo("testprofile",info);
  printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n",info.openssl_config,info.certs,info.name,info.reqs,info.keys,info.ca,info.logs);
  */
  if(gpki::db::profile_exists("testprofile")){
    printf("testprofile exists\n");
  }
  if(gpki::db::profile_exists("wiski")){
    printf("wiski exists\n");
  }
  return 0;
}
