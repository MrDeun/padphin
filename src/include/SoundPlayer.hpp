#pragma once
#include <SDL2/SDL_mixer.h>
#include <fmtlog/fmtlog.h>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class SoundPlayer {
private:
  std::vector<Mix_Chunk *> sounds{};
  bool initialized = false;

public:
  SoundPlayer();
  ~SoundPlayer();

  void clear();

  size_t load_sound(const fs::path& path);
  void play_sound(size_t sound_id) const;

};