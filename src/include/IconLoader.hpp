#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

struct Icon {
  uint32_t texture_id = 0;
  float width = 0.0f;
  float height = 0.0f;
};

class IconLoader {
public:
  IconLoader() = default;
  ~IconLoader();

  IconLoader(const IconLoader &) = delete;
  IconLoader &operator=(const IconLoader &) = delete;

  Icon load(const std::string &svg_path, float scale = 1.0f);
  void clear();

private:
  std::unordered_map<std::string, Icon> cache_;
};
