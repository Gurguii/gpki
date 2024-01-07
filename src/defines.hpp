#pragma once
#include <filesystem>

#define setaction(x) gpki::globals::action_ptr = x
#define seterror(x) gpki::globals::lasterror = x

template <typename T> int IS_ABSOLUT_PATH(T path) {
#ifdef _WIN32
  return std::isalpha(path[0]);
#else
  return (path[0] == '/');
#endif
};

#ifdef _WIN32
/* WINDOWS STUFF */
#define CURRENT_PATH std::filesystem::current_path().string()
#define SLASH "\\"
#else
/* LINUX STUFF */
#define CURRENT_PATH std::filesystem::current_path()
#define SLASH std::string("/")
#endif
