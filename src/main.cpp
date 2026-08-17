#include "SFML/Graphics/Font.hpp"
#include "include/App.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <map>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  sf::RenderWindow win(sf::VideoMode({2560, 1440}),
                       "Padphin - File Explorer for a gamepad");
  win.setFramerateLimit(24);
  fs::path begin_path(".");
  sf::Font font("jetbrains.ttf");
  App app(font);
  app.go_to(begin_path);
  while (win.isOpen()) {
    app.event_loop(win);
    app.update_state();
    win.clear(sf::Color(30, 30, 30));
    auto tex = app.render();
    tex.display();
    sf::Sprite sprite(tex.getTexture());
    win.draw(sprite);
    win.display();
  }
  fmt::println("hello world!");
  return 0;
}
