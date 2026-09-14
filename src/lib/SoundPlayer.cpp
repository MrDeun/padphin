#include "SoundPlayer.hpp"
#include "App.hpp"

#include <SDL2/SDL_mixer.h>
#include <fmtlog/fmtlog.h>


size_t SoundPlayer::load_sound(const fs::path &path) {
  auto full_path = find_resource_path(path);
  auto sound = Mix_LoadWAV(full_path.c_str());
  if (!sound) {
    loge("Error: failure to load sound effect '{}' - Reason {}",
         path.string(), SDL_GetError());
    return -1;
  }
  sounds.push_back(sound);
  return sounds.size() - 1;
}

void SoundPlayer::play_sound(size_t sound_id) const {
  std::error_code ec;
  Mix_Chunk *selected_audio = nullptr;
  try {
    selected_audio = sounds.at(sound_id);
  } catch (const std::exception &ex) {
    logw("Error: Requested audio was not found - ID={}", sound_id);
  }
  Mix_PlayChannel(-1, selected_audio, 1);
}