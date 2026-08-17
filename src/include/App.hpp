#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/Text.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
class App {
  enum class Dir: uint8_t { None, Up, Down, Left, Right };

  Dir current_direction = Dir::None;
  fs::path current_path = ".";
  std::vector<fs::directory_entry> entries;
  size_t selected_index = 0;
  sf::Text cell_label;

  static constexpr float cell_width = 120.0f;
  static constexpr float cell_height = 120.0f;
  static constexpr float padding = 10.0f;
  static constexpr uint font_size = 20;

  void populate_entries() {
    entries.clear();
    for (const auto &e : fs::directory_iterator(current_path))
      entries.push_back(e);
    std::sort(std::begin(entries),std::end(entries),[](const fs::directory_entry& first, const fs::directory_entry& second){
        auto first_result = static_cast<uint8_t>(first.is_directory());
        auto second_result = static_cast<uint8_t>(second.is_directory());
        return first_result > second_result;
    });
    entries.shrink_to_fit();
  }

public:
  App() = delete;
  App(const sf::Font &font) : cell_label(font) {
    cell_label.setCharacterSize(font_size);
  }
  void go_to(fs::path new_path) {
    current_path = new_path;
    populate_entries();
  }
  
  const fs::path &get_current_path() const { return current_path; }
  void update_state();
  void event_loop(sf::RenderWindow& win);
  sf::RenderTexture render();
};