#include "SFML/Graphics/Font.hpp"
#include "include/App.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <map>

namespace fs = std::filesystem;

void event_loop(sf::Window &win, App& app) {

}

int main(int argc, char **argv) {
  sf::RenderWindow win(sf::VideoMode({1920, 1080}),
                       "Padphin - File Explorer for a gamepad");
  fs::path begin_path(".");
  sf::Font font("jetbrains.ttf");
  App app(font);
  app.go_to(begin_path);
  while (win.isOpen()) {
    app.event_loop(win);
    win.clear(sf::Color(30, 30, 30));
    app.render(win);
    win.display();
  }
  fmt::println("hello world!");
  return 0;
}
