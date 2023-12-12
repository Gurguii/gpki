#include <filesystem>
#include <fstream>
bool hasWritePermissions(const std::filesystem::path &directoryPath) {
  /* this approach is kind of sad to see */
  try {
    auto s = std::filesystem::status(directoryPath);
    return true;
  } catch (std::exception &ex) {
    return false;
  }
}
