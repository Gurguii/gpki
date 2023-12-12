#include <filesystem>
#include <fstream>
bool hasWritePermissions(const std::filesystem::path &directoryPath) {
  try {
    std::filesystem::status(directoryPath);
    return true;
  } catch (std::exception &ex) {
    return false;
  }
}
