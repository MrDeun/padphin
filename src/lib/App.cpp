#include "../include/App.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

void App::event_loop(sf::RenderWindow& win){
      while (const auto event = win.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      win.close();
    }
  }
}

 void App::render(sf::RenderWindow &win) {
    int columns =
        static_cast<int>((win.getSize().x - padding) / (cell_width + padding));
    if (columns < 1)
      columns = 1;
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
      int col = static_cast<int>(i) % columns;
      int row = static_cast<int>(i) / columns;
      float x = padding + col * (cell_width + padding);
      float y = padding + row * (cell_height + padding);

      sf::RectangleShape cell(sf::Vector2f(cell_width, cell_height));
      cell.setPosition({x, y});
      cell.setFillColor(sf::Color(50, 50, 50));

      bool isSelected = (static_cast<int>(i) == selected_index);
      cell.setOutlineColor(isSelected ? sf::Color::Cyan
                                      : sf::Color(80, 80, 80));
      cell.setOutlineThickness(isSelected ? 3.f : 1.f);
      win.draw(cell);

      sf::RectangleShape icon(sf::Vector2f(40.f, 40.f));
      icon.setPosition({x + (cell_width - 40.f) / 2.f, y + 10.f});
      if (e.is_directory()) {
        icon.setFillColor(sf::Color(230, 200, 80));
      } else {
        icon.setFillColor(sf::Color::Transparent);
        icon.setOutlineColor(sf::Color(180, 180, 180));
        icon.setOutlineThickness(2.f);
      }
      win.draw(icon);

      cell_label.setPosition({x, y + cell_height});
      cell_label.setString(e.path().stem().string());
      win.draw(cell_label);
    }
  }