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

struct ProfileInfo
{
  char name[GPKI_MAX_PATH]{};
  char keys[GPKI_MAX_PATH]{};
  char certs[GPKI_MAX_PATH]{};
  char ca[GPKI_MAX_PATH]{};
  char reqs[GPKI_MAX_PATH]{};
  char serial[GPKI_MAX_PATH]{};
  char x509[GPKI_MAX_PATH]{};
  char templates[GPKI_MAX_PATH]{};
  char openssl_config[GPKI_MAX_PATH]{};
  char logs[GPKI_MAX_PATH]{};
};
/*
struct ProfileInfo
{
  char *name;
  char *keys;
  char *certs;
  char *ca;
  char *reqs;
  char *serial;
  char *x509;
  char *templates;
  char *openssl_config;
  char *logs;
};
*/
struct CertCreationInfo {
  char serial[10];
  char csr[512];
  char crt[512];
  char key[512];
  char csr_command[2048];
  char crt_command[2048];
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
  static const char *& get_error();

public:
  // Constructor
  static int initialize(const char *dbpath);
  
  // Crud
  static int insert_profile(ProfileInfo &pinfo);
  static int select_profile(std::string_view profile_name);
  static int delete_profile(std::string_view profile_name);
  static int update_database(std::string_view profile_name, std::unordered_map<std::string,std::string> values);

  // Printing
  static int display_profiles();

  // Structure population
  static int populate_CertCreationInfo(std::string_view profile_name, CertCreationInfo &buff);
  static int populate_ProfileInfo(std::string_view profile_name, ProfileInfo &buff);
  
  // 
  static int profile_exists(std::string_view profile_name);
};
} // namespace gpki
#endif
