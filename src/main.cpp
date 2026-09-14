#include "GamepadControl.hpp"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

#include <GL/gl.h>

#include "include/App.hpp"
#include "include/CompositeControl.hpp"
#include "include/KeyboardControl.hpp"

#include <fmtlog/fmtlog.h>
#include <memory>

#include <SDL2/SDL.h>

#include <filesystem>
#include <system_error>

int main(int arg_count, char **args) {
  fmtlog::setLogLevel(fmtlog::DBG);
  fmtlog::startPollingThread();

  fs::path initial_path = fs::absolute(".");
  if (arg_count > 1) {
    std::error_code ec{};
    initial_path = fs::absolute(args[1], ec);
    if (ec || initial_path.has_filename()) {
      logw("WARNING: Error processioning inputted path. Defaulting to "
           "'.'. Reason: {}",
           ec.value() == 0 ? "Path is a has a filename, expected directory path" :ec.message());
      initial_path = fs::absolute(".");
    }
  }
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0) {
    loge("SDL_Init failed: {}", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window =
      SDL_CreateWindow("Padphin - File Explorer for a gamepad",
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920,
                       1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (!window) {
    loge("SDL_CreateWindow failed: {}", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (!gl_context) {
    loge("SDL_GL_CreateContext failed: {}", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_GL_SetSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  auto font_path = find_resource_path("font/PressStart2P.ttf");
  io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f);

  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");

  auto gamepad = std::make_unique<GamepadControl>();
  GamepadControl *gamepad_ptr = gamepad.get();

  App app(std::make_unique<CompositeControl>([&] {
    std::vector<std::unique_ptr<IControl>> v;
    v.push_back(std::make_unique<KeyboardControl>(window));
    v.push_back(std::move(gamepad));
    return std::move(v);
  }()));
  app.go_to(std::move(initial_path));

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        running = false;
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE &&
          (event.key.keysym.mod & KMOD_ALT))
        running = false;
      if (event.type == SDL_CONTROLLERDEVICEADDED)
        gamepad_ptr->on_controller_connected(event.cdevice.which);
      if (event.type == SDL_CONTROLLERDEVICEREMOVED)
        gamepad_ptr->on_controller_disconnected(event.cdevice.which);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    app.poll_input();
    app.update_state();
    app.render();

    ImGui::Render();
    int display_w = 0, display_h = 0;
    SDL_GL_GetDrawableSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
