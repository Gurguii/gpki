#pragma once
#include "../defines.hpp"
#include "../globals.hpp"
#include "../structs.hpp"
#include <cstdint>
#include <fstream>
#include <sqlite3.h>

#define STARTING_CRLNUMBER "1000\n"
#define STARTING_SERIAL "01\n"
#define TABLES                                                                 \
  { "profiles", "entitites" }

#define SQL_CREATE_TABLES                                                      \
  R"(CREATE TABLE profiles (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  rootdir TEXT NOT NULL UNIQUE
);
CREATE TABLE entities(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  cn TEXT NOT NULL,
  serial INTEGER NOT NULL,
  cert TEXT NOT NULL,
  key TEXT NOT NULL,
  csr TEXT NOT NULL,
  profile_id INTEGER NOT NULL,
  FOREIGN KEY (profile_id) REFERENCES profiles(id)
);)"

#define dbpath globals::metainfo::database
#define dbpath_c globals::metainfo::database.c_str()

namespace gpki::database {

static inline sqlite3 *db = nullptr;

static inline int open_db() {
  if (sqlite3_open(dbpath_c, &database::db) != SQLITE_OK) {
    seterror("couldn't open database");
    return -1;
  }
  return 0;
};
static inline int close_db() {
  if (sqlite3_close(database::db) != SQLITE_OK) {
    seterror("couldn't close database");
    return -1;
  }
  return 0;
}

static inline int exec(std::string_view sql,
                       int (*callback)(void *ptr, int ncols, char **cvals,
                                       char **cheads) = nullptr,
                       void *ptr = nullptr, char **errmsg = nullptr) {
  if (database::open_db()) {
    seterror("couldn't open db\n");
    return -1;
  }
  std::cout << "executing : " << sql << "\n";
  sqlite3_exec(database::db, std::string(sql).c_str(), callback, ptr, errmsg);
  if (database::close_db()) {
    seterror("couldn't close db\n");
    return -1;
  }
  return 0;
}

static inline int exec_v2(std::string_view sql,int(*row_callback)(const char *colname, const unsigned char *colvalue) = nullptr){
  sqlite3_stmt *ptr;
  int code = 0;
  std::cout << "executign: " << sql << "\n";
  if(database::open_db()) {
    std::cout << "couldn't open db\n";
    return -1;
  }
  int errcode = 0;
  // sqlite3_prepare(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail)
  if((errcode = sqlite3_prepare_v2(database::db,sql.data(),sql.size(),&ptr,NULL)) != SQLITE_OK){
    std::cout << "couldn't prepare sql statement -> '" << sql << "'\n";
    std::cout << "errcode -> " << errcode << "\n";
    return -1;
  };
  errcode = sqlite3_step(ptr);
  std::cout << "errcode -> " << errcode << "\n";
  switch(errcode){
    case SQLITE_CONSTRAINT:
      seterror("SQLITE_CONSTRAINT\n");
      break;
    case SQLITE_DONE:
      seterror("statement executed\n");
      break;
    case SQLITE_ERROR:
      seterror("statement got a generic error - no specific info about it\n");
      break;
    case SQLITE_ROW:
      if(row_callback == nullptr){
        seterror("callback function NULL in exec_vs()\n");
        code = -1;
        break;
      }
      int ncols = sqlite3_column_count(ptr);
      for(int i = 0; i < ncols; ++i){
        row_callback(sqlite3_column_name(ptr,i),sqlite3_column_text(ptr,i));
      }
     break;
  }
  sqlite3_finalize(ptr);
  return code;
}

static inline int initialize() {
  sqlite3_open(dbpath_c, &database::db);
  if (!std::filesystem::exists(dbpath)) {
    // Database path does not exist, try to create it
    seterror("wasn't able to create database at '" + dbpath + "'");
    std::cout << "OK\n";
  }
  database::exec(SQL_CREATE_TABLES);
  return 0;
}
} // namespace gpki::database

namespace gpki::database::profiles {
/* Crud */
static int insert(Profile &profile);
static int remove(Profile &profile);
static int select(Profile &profile);
static int update(Profile &profile);
/* Checks */
static int exists(Profile &profile);
static int exists(std::string_view profile_name);
/* Populate struct Profile */
static int get(std::string_view profile_name, Profile *ptr);
} // namespace gpki::database::profiles

namespace gpki::database::entities {
/* Crud */
static int insert(Entity &entity);
static int remove_all(Profile &pinfo);
static int select(Entity &entity);
static int update(Entity &entity);

/* Checks */
static int exists(Entity &entity);
static int get(std::string_view cn, int profile_id, Entity *ptr);
} // namespace gpki::database::entities
