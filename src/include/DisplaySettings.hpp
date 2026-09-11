#pragma once

// Central list of supported display resolutions plus the UI scale factor
// derived from them. All modes are 16:9, so width/1920 == height/1080.
struct DisplayMode {
  int width = 0;
  int height = 0;
  const char *label = nullptr;
};

namespace DisplaySettings {

// Ordered from smallest to largest. Index 1 (1920x1080) is the default.
inline constexpr DisplayMode kResolutions[] = {
    {1280, 720, "1280x720"},
    {1920, 1080, "1920x1080"},
    {2560, 1440, "2560x1440"},
    {3840, 2160, "3840x2160 (4K)"},
};

inline constexpr int kCount = sizeof(kResolutions) / sizeof(kResolutions[0]);
inline constexpr int kDefaultIndex = 1;

inline constexpr float kBaseWidth = 1920.0f;
inline constexpr float kBaseHeight = 1080.0f;

// UI scale relative to the 1080p baseline: 720p -> 0.667, 1080p -> 1.0,
// 1440p -> 1.333, 4K -> 2.0. Uses the smaller axis so custom (non 16:9)
// window sizes still scale sensibly.
inline float ui_scale_for(int width, int height) {
  if (width <= 0 || height <= 0)
    return 1.0f;
  float sx = static_cast<float>(width) / kBaseWidth;
  float sy = static_cast<float>(height) / kBaseHeight;
  return sx < sy ? sx : sy;
}

// Returns the index into kResolutions matching w/h, or -1 if custom.
inline int index_for(int width, int height) {
  for (int i = 0; i < kCount; ++i) {
    if (kResolutions[i].width == width && kResolutions[i].height == height)
      return i;
  }
  return -1;
}

} // namespace DisplaySettings
