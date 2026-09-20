#pragma once
#include <fmtlog/fmtlog.h>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <system_error>
namespace fs = std::filesystem;

class Clipboard {
public:
  const std::vector<fs::path> &items = _clipboard;
  size_t size() const { return _clipboard.size(); }
  void move_files_to(const fs::path &);
  void copy_files_to(const fs::path &);
  void append(const fs::path &_path);
  void append(fs::path &&_path); 
  void delete_files(bool recursive);
private:
  static void remove(const fs::path& _path) {
    std::error_code ec{};
    fs::remove_all(_path, ec);
    if (ec) {
      loge("Error during remove all... {}", ec.message());
    }
  };
  static void remove_all(const fs::path& _path) {
    std::error_code ec{};
    fs::remove(_path, ec);
    if (ec) {
      loge("Error during remove... {}", ec.message());
    }
  };
  std::vector<fs::path> _clipboard{};
};