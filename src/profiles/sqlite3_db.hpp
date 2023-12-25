#pragma once
#ifdef _WIN32
#define GPKI_MAX_PATH MAX_PATH
#else
#define GPKI_MAX_PATH 4096
#include <iostream>
#include <string_view>
#include <string>
#include <unordered_map>
#include <cstring>
#include <sqlite3.h>
#include <fstream>
#include "../globals.hpp"

#define DEFAULT_CRLNUMBER "1000"
#define DEFAULT_SERIAL "1"
#define DEFAULT_SUBJECT "/C=ES/ST=CANARIAS/L=LAS PALMAS/O=MARIWANOS/CN=%s/emailAddress=noemail"

struct ProfileInfo
{
  std::string name{};
  std::string source_dir{};
  std::string keys{};
  std::string certs{};
  std::string ca{};
  std::string reqs{};
  std::string serial{};
  std::string x509{};
  std::string templates{};
  std::string openssl_config{};
  std::string logs{};
  std::string database{};
  std::string crl{};
};
/*
struct ProfileInfo
{
  std::string *name;
  std::string *keys;
  std::string *certs;
  std::string *ca;
  std::string *reqs;
  std::string *serial;
  std::string *x509;
  std::string *templates;
  std::string *openssl_config;
  std::string *logs;
};
*/
struct CertCreationCommands {
  std::string csr_command{};
  std::string crt_command{};
};
namespace gpki
{
  class db{
private:
  /* Properties */
  static const char* db_path;
  static const char *lasterror;
  static sqlite3 *_db;
  /* Methods */
  static int open_db();
  static int close_db();
    /* Internal function used by db::insert_profile() to create profile files */
  static int create_files(ProfileInfo *pinfo, std::string_view src_config_dir, std::string_view dst_config_dir);
public:
  // Constructor
  static int initialize(const char *dbpath);
  
  // Crud
  static int insert_profile(ProfileInfo &pinfo,std::string_view src_config_dir, std::string_view dst_config_dir);
  static int select_profile(std::string_view profile_name);
  static int delete_profile(std::string_view profile_name);
  static int update_database(std::string_view profile_name, std::unordered_map<std::string,std::string> values);

  // Printing
  static int display_profiles();
  static const char *& get_error();

  // Structure population
  static int populate_CertCreationCommands(ProfileInfo *ptr, std::string_view profile_name, CertCreationCommands &buff);
  static int populate_ProfileInfo(std::string_view profile_name, ProfileInfo &buff);
  
  // 
  static int profile_exists(std::string_view profile_name);
};
} // namespace gpki
#endif
