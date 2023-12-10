#include <filesystem>
#include <fstream>
bool hasWritePermissions(const std::filesystem::path &directoryPath) {
  std::ofstream file(directoryPath);
  if (file.is_open()) {
    file.close();
    std::filesystem::remove(directoryPath);
    return true;
  }
  return false;
}
