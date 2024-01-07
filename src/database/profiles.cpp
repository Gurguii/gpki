#include "database.hpp"

namespace gpki::database::profiles::callbacks {
int exists(void *ptr, int ncols, char **vals, char **headers) {
  *(int *)ptr = 1;
  return 0;
}
int populate_Profile(void *ptr, int ncols, char **vals, char **headers) {
  Profile &ref = *(Profile *)ptr;
  ref.id = strtol(*vals, nullptr, 10);
  ref.name = *(vals + 1);
  ref.source_dir = *(vals + 2);
  return 0;
}
} // namespace gpki::database::profiles::callbacks

using namespace gpki;
int database::profiles::exists(Profile &profile) {
  int code = 0;
  std::string sql = "SELECT * FROM profiles WHERE name='" + profile.name + "'";
  database::exec(sql, database::profiles::callbacks::exists, &code);
  return code;
}
int database::profiles::exists(std::string_view profile_name) {
  int code = 0;
  std::string sql =
      "SELECT * FROM profiles WHERE name = '" + std::string(profile_name) + "'";
  database::exec(sql, database::profiles::callbacks::exists, &code);
  return code;
}
int database::profiles::insert(Profile &profile) {
  if (profile.name.empty() || profile.source_dir.empty()) {
    seterror("profile name | source_dir cannot be empty...");
    return -1;
  }
  std::string sql = "INSERT INTO profiles (name,rootdir) VALUES ('" +
                    PROFILE.name + "', '" + PROFILE.source_dir + "')";
  database::exec(sql);
  return 0;
}

int database::profiles::get(std::string_view profile_name, Profile *ptr) {
  std::string sql =
      "SELECT * FROM profiles WHERE name = '" + std::string(profile_name) + "'";
  database::exec(sql, database::profiles::callbacks::populate_Profile, ptr);
  return 0;
}

int database::profiles::remove(Profile &profile){
  std::string sql = "DELETE FROM profiles WHERE name = '" + std::string(profile.name) + "'";
  if(database::exec_v2(sql) != SQLITE_DONE){
    return -1;
  };
  return 0;
}
