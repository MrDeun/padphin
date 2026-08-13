#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <map>

namespace fs = std::filesystem;
struct GridItem {
  fs::directory_entry entry;
  sf::Text label;
};

enum class Dir { None, Up, Down, Left, Right };

void event_loop(sf::Window &win) {
  while (const auto event = win.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      win.close();
    }
  }
}

int main(int argc, char **argv) {
  sf::RenderWindow win(sf::VideoMode({1920, 1080}),
                       "Padphin - File Explorer for a gamepad");
  fs::path current_path(".");
  sf::Font font("jetbrains.ttf");
  std::vector<fs::directory_entry> entries;
  for (const auto &e : fs::directory_iterator("."))
    entries.push_back(e);
  constexpr float cell_width = 120.0f, cell_height = 90.0f, padding = 10.0f;
  int columns =
      static_cast<int>((win.getSize().x - padding) / (cell_width + padding));
  if (columns < 1)
    columns = 1;

  std::vector<GridItem> items;
  for (const auto &entry : entries) {
    sf::Text text(font);
    text.setString(entry.path().filename().string());
    text.setCharacterSize(14);
    text.setFillColor(entry.is_directory() ? sf::Color::Yellow
                                           : sf::Color::White);
    items.push_back({entry, text});
  }
  int rows = (static_cast<int>(items.size()) + columns - 1) / columns;
  int selected = 0;
  if (items.empty())
    selected = -1;
  while (win.isOpen()) {

    event_loop(win);

    win.clear(sf::Color(30, 30, 30));

    for (size_t i = 0; i < items.size(); ++i) {
      int col = static_cast<int>(i) % columns;
      int row = static_cast<int>(i) / columns;
      float x = padding + col * (cell_width + padding);
      float y = padding + row * (cell_height + padding);

      sf::RectangleShape cell(sf::Vector2f(cell_width, cell_height));
      cell.setPosition({x, y});
      cell.setFillColor(sf::Color(50, 50, 50));

      bool isSelected = (static_cast<int>(i) == selected);
      cell.setOutlineColor(isSelected ? sf::Color::Cyan
                                      : sf::Color(80, 80, 80));
      cell.setOutlineThickness(isSelected ? 3.f : 1.f);
      win.draw(cell);

      sf::RectangleShape icon(sf::Vector2f(40.f, 40.f));
      icon.setPosition({x + (cell_width - 40.f) / 2.f, y + 10.f});
      if (items[i].entry.is_directory()) {
        icon.setFillColor(sf::Color(230, 200, 80));
      } else {
        icon.setFillColor(sf::Color::Transparent);
        icon.setOutlineColor(sf::Color(180, 180, 180));
        icon.setOutlineThickness(2.f);
      }
      win.draw(icon);

      items[i].label.setPosition({x, y + cell_height - 20.f});
      win.draw(items[i].label);
    }
    win.display();
  }
  fmt::println("hello world!");
  return 0;
}
