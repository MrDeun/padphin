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
  void append(const fs::path &_path) {
    auto it = std::ranges::find(_clipboard, _path);
    if (it == _clipboard.end()) {
      _clipboard.emplace_back(_path);
    }
  }
  void append(fs::path &&_path) {
    auto it = std::ranges::find(_clipboard, _path);
    if (it == _clipboard.end()) {
      _clipboard.emplace_back(std::move(_path));
    }
  }
  void delete_files(bool recursive = false) {
    std::error_code ec{};
    if (recursive) {
      std::for_each(std::begin(_clipboard), std::end(_clipboard), remove_all);
    } else {
      std::for_each(std::begin(_clipboard), std::end(_clipboard), remove);
    }
  }

private:
  std::function<void(const fs::path &)> remove_all = [](const fs::path &_path) {
    std::error_code ec{};
    fs::remove_all(_path, ec);
    if (ec) {
      loge("Error during remove all... {}", ec.message());
    }
  };
  std::function<void(const fs::path &)> remove = [](const fs::path &_path) {
    std::error_code ec{};
    fs::remove(_path, ec);
    if (ec) {
      loge("Error during remove... {}", ec.message());
    }
  };
  std::vector<fs::path> _clipboard{};
};