#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Icon {
  uint32_t texture_id = 0;
  float width = 0.0f;
  float height = 0.0f;
};

struct IconSet{
  Icon up;
  Icon down;
  Icon left;
  Icon right;
  Icon accept;
  Icon deny;
  Icon add_to_clipboard;
  Icon open_menubar;
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
  std::vector<uint32_t> textures_ids;
};
