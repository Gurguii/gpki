#include "database.hpp"

namespace gpki::database::entities::callbacks {
int exists(void *ptr, int ncols, char **vals, char **headers) {
  *(int *)ptr = 1;
  return 0;
}
int populate_Entity(void *ptr, int ncol, char **vals, char **headers) {
  Entity &ref = *(Entity *)ptr;
  ref.subject.cn = *(vals + 1);
  ref.serial = *(vals + 2);
  ref.cert_path = *(vals + 3);
  ref.key_path = *(vals + 4);
  ref.csr_path = *(vals + 5);
  ref.profile_id = strtol(*(vals + 6), nullptr, 10);
  return 0;
}
} // namespace gpki::database::entities::callbacks
using namespace gpki;
int database::entities::exists(Entity &entity) {
  int code = 0;
  std::string sql =
      "SELECT * FROM entities where cn = '" + entity.subject.cn + "'";
  database::exec(sql, database::entities::callbacks::exists, &code);
  return code;
};

int database::entities::insert(Entity &entity) {
  std::string sql = "INSERT INTO entities (cn,serial,cert,key,csr,profile_id) VALUES (";
  sql += "'" + entity.subject.cn + "',";
  sql += "'" + entity.serial + "',";
  sql += "'" + entity.cert_path + "',";
  sql += "'" + entity.key_path + "',";
  sql += "'" + entity.csr_path + "',";
  sql += "'" + std::to_string(entity.profile_id)+"')";
  return database::exec(sql);
  //return database::exec(sql);
};

int database::entities::get(std::string_view cn, int profile_id, Entity *ptr) {
  std::string sql = "SELECT * FROM entities WHERE cn = '" + (*ptr).subject.cn +
                    "'" + "AND profile_id = '" + std::to_string(profile_id) +
                    "'";
  exec(sql, entities::callbacks::populate_Entity);
  return 0;
}

int database::entities::remove_all(Profile &pinfo)
{
  std::string sql = "DELETE FROM entities WHERE profile_id = '" + std::to_string(pinfo.id) + "'";
  exec(sql);
  return 0;
}
