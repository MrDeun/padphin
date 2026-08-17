#include "../include/App.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Window/Joystick.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "fmt/base.h"

void App::event_loop(sf::RenderWindow &win) {
  while (const auto event = win.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      win.close();
    }
  }

  auto left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
  auto right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
  auto up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
  auto down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

  int hor_direction = right - left;
  int ver_direction = up - down;

  if (!hor_direction) {
    if (ver_direction == 1) {
      current_direction = Dir::Up;
    } else if (ver_direction == -1) {
      current_direction = Dir::Down;
    } else {
      current_direction = Dir::None;
    }
  } else {
    if (hor_direction == 1) {
      current_direction = Dir::Right;
    } else if (hor_direction == -1) {
      current_direction = Dir::Left;
    }
  }
}

void App::update_state() {
  switch (current_direction) {
  case Dir::None:
    break;
  case Dir::Up:
    break;
  case Dir::Down:
    break;
  case Dir::Left:
    selected_index = (selected_index - 1) % entries.size();
    break;
  case Dir::Right:
    selected_index = (selected_index + 1) % entries.size();
    break;
  }
}

sf::RenderTexture App::render() {
  sf::RenderTexture texture({1600, 900});
  int columns = static_cast<int>((texture.getSize().x - padding) /
                                 (cell_width + padding));
  if (columns < 1)
    columns = 1;
  sf::RectangleShape cell(sf::Vector2f(cell_width, cell_height));
  sf::RectangleShape icon(sf::Vector2f(40.f, 40.f));
  for (size_t i = 0; i < entries.size(); ++i) {
    const auto &e = entries[i];
    int col = static_cast<int>(i) % columns;
    int row = static_cast<int>(i) / columns;
    float x = padding + col * (cell_width + padding);
    float y = padding + row * (cell_height + padding);

    cell.setPosition({x, y});
    cell.setFillColor(sf::Color(50, 50, 50));

    bool isSelected = (static_cast<int>(i) == selected_index);
    cell.setOutlineColor(isSelected ? sf::Color::Cyan : sf::Color(80, 80, 80));
    cell.setOutlineThickness(isSelected ? 3.f : 1.f);
    texture.draw(cell);

    icon.setPosition({x + (cell_width - 40.f) / 2.f, y + 10.f});
    if (e.is_directory()) {
      icon.setFillColor(sf::Color(230, 200, 80));
    } else {
      icon.setFillColor(sf::Color::Transparent);
      icon.setOutlineColor(sf::Color(180, 180, 180));
      icon.setOutlineThickness(2.f);
    }
    texture.draw(icon);

    cell_label.setPosition({x, y + cell_height});
    cell_label.setString(e.path().stem().string());
    texture.draw(cell_label);
  }

  return std::move(texture);
}