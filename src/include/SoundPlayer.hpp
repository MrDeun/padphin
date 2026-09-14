#pragma once
#include <SDL2/SDL_mixer.h>
#include <fmt/format.h>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class SoundPlayer {
private:
  std::vector<Mix_Chunk *> sounds{};
  bool initialized = false;

public:
  SoundPlayer() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0) {
      initialized = true;
    } else {
      fmt::println("Failed to open audio: {}", Mix_GetError());
    }
  }
  ~SoundPlayer() {
    clear();
    if (initialized) {
      Mix_CloseAudio();
    }
  }

  void clear() {
    for (auto ptr : sounds) {
      Mix_FreeChunk(ptr);
    }
    sounds.clear();
  }

  size_t load_sound(const fs::path& path);
  void play_sound(size_t sound_id) const;

};