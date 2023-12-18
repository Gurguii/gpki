#include <cctype>

#define STARTING_CRLNUMBER "1000\n"
#define STARTING_SERIAL "01\n"

const auto IS_ABSOLUT_PATH = [](const char *path) {
#ifdef _WIN32
  return std::isalpha(*path);
#else
  return (*path == '/');
#endif
};

#ifdef _WIN32
/* WINDOWS STUFF */
#define CURRENT_PATH std::filesystem::current_path().string()
#define SLASH "\\"
#else
/* LINUX STUFF */
#define CURRENT_PATH std::filesystem::current_path()
#define SLASH "/"
#endif
