#include <Clipboard.hpp>

void Clipboard::copy_files_to(const fs::path &new_path) {
  std::ranges::for_each(_clipboard, [&new_path](const fs::path &old_path) {
    fs::copy(old_path, new_path);
  });
  _clipboard.clear();
}
void Clipboard::append(const fs::path &_path) {
  auto it = std::ranges::find(_clipboard, _path);
  if (it == _clipboard.end()) {
    _clipboard.emplace_back(_path);
  }
}
void Clipboard::append(fs::path &&_path) {
  auto it = std::ranges::find(_clipboard, _path);
  if (it == _clipboard.end()) {
    _clipboard.emplace_back(std::move(_path));
  }
}
void Clipboard::move_files_to(const fs::path &new_path) {
  std::vector<fs::path> new_paths(_clipboard.size());
  std::ranges::transform(_clipboard, new_paths.begin(),
                         [&new_path](const fs::path &_path) {
                           return (new_path / _path.filename());
                         });
  for (size_t i = 0; i < _clipboard.size(); ++i) {
    const auto &old_path = _clipboard[i];
    const auto &new_path = new_paths[i];
    fs::rename(old_path, new_path);
  }
}
void Clipboard::delete_files(bool recursive = false) {
  std::error_code ec{};
  if (recursive) {
    std::for_each(std::begin(_clipboard), std::end(_clipboard), remove_all);
  } else {
    std::for_each(std::begin(_clipboard), std::end(_clipboard), remove);
  }
}