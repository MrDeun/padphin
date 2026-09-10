#include "../include/IconLoader.hpp"

#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#include <GL/gl.h>
#include <cstdio>
#include <cstring>
#include <vector>

IconLoader::~IconLoader() { clear(); }

void IconLoader::clear() {
  for (auto &[key, cached] : cache_) {
    if (cached.texture_id) {
      glDeleteTextures(1, &cached.texture_id);
    }
  }
  cache_.clear();
}

Icon IconLoader::load(const std::string &svg_path, float scale) {
  auto it = cache_.find(svg_path);
  if (it != cache_.end()) {
    return {it->second.texture_id, it->second.width, it->second.height};
  }

  NSVGimage *image = nsvgParseFromFile(svg_path.c_str(), "px", 96);
  if (!image) {
    return {};
  }

  int w = static_cast<int>(image->width * scale);
  int h = static_cast<int>(image->height * scale);
  if (w <= 0 || h <= 0) {
    nsvgDelete(image);
    return {};
  }

  std::vector<unsigned char> pixels(w * h * 4, 0);

  NSVGrasterizer *rast = nsvgCreateRasterizer();
  if (!rast) {
    nsvgDelete(image);
    return {};
  }

  nsvgRasterize(rast, image, 0, 0, scale, pixels.data(), w, h, w * 4);
  nsvgDeleteRasterizer(rast);
  nsvgDelete(image);

  // Flip vertically (OpenGL expects bottom-left origin).
  int stride = w * 4;
  std::vector<unsigned char> flipped(stride * h);
  for (int y = 0; y < h; ++y) {
    std::memcpy(&flipped[y * stride], &pixels[(h - 1 - y) * stride], stride);
  }

  // Premultiply alpha for OpenGL.
  for (int i = 0; i < w * h; ++i) {
    unsigned char a = flipped[i * 4 + 3];
    flipped[i * 4 + 0] =
        static_cast<unsigned char>(flipped[i * 4 + 0] * a / 255);
    flipped[i * 4 + 1] =
        static_cast<unsigned char>(flipped[i * 4 + 1] * a / 255);
    flipped[i * 4 + 2] =
        static_cast<unsigned char>(flipped[i * 4 + 2] * a / 255);
  }

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, flipped.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  cache_[svg_path] = {tex, static_cast<float>(w), static_cast<float>(h)};
  return {tex, static_cast<float>(w), static_cast<float>(h)};
}
