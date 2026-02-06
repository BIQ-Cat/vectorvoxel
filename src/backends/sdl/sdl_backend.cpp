#include <cstring>
#include <stdexcept>
#include <vectorvoxel/backends/sdl.h>

namespace VectorVoxel::Backends {
SDLBackend::SDLBackend(SDL_Window *window, bool vsync)
    : window(window), use_vsync(vsync) {
  if (!window) {
    throw std::runtime_error("SDLBackend: window is nullptr");
  }

  Uint32 renderer_flags = SDL_RENDERER_ACCELERATED;
  if (use_vsync) {
    renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
  }

  renderer = SDL_CreateRenderer(window, -1, renderer_flags);
  if (!renderer) {
    throw std::runtime_error(
        std::string("SDLBackend: failed to create renderer: ") +
        SDL_GetError());
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  last_width = w;
  last_height = h;
}

SDLBackend::~SDLBackend() { cleanup(); }

void SDLBackend::cleanup() {
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }

  // WARNING: DO NOT TOUCH WINDOW - NOT OUR PROPERTY
}

bool SDLBackend::isReady() { return renderer != nullptr; }

void SDLBackend::restoreTexture(int width, int height) {
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!texture) {
    throw std::runtime_error(
        std::string("SDLBackend: failed to create texture: ") + SDL_GetError());
  }

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

  last_width = width;
  last_height = height;
}

void SDLBackend::renderFrame(const std::vector<uint32_t> &pixels, int width,
                             int height) {
  if (!isReady() || pixels.empty())
    return;

  if (!texture || width != last_width || height != last_height) {
    restoreTexture(width, height);
  }

  void *texture_pixels = nullptr;
  int line_size = 0;
  if (SDL_LockTexture(texture, nullptr, &texture_pixels, &line_size) == 0) {
    if (line_size == width * 4) {
      memcpy(texture_pixels, pixels.data(), pixels.size() * 4);
    } else {
      uint8_t *dst = static_cast<uint8_t *>(texture_pixels);
      const uint8_t *src = reinterpret_cast<const uint8_t *>(pixels.data());
      int src_line_size = width * 4;

      for (int y{}; y < height; ++y) {
        memcpy(dst, src, src_line_size);
        dst += line_size;
        src += src_line_size;
      }
    }
    SDL_UnlockTexture(texture);
  } else {
    SDL_UpdateTexture(texture, nullptr, pixels.data(),
                      width * sizeof(uint32_t));
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_Rect screen_area{0, 0, last_width, last_height};
  SDL_RenderCopy(renderer, texture, nullptr, &screen_area);

  SDL_RenderPresent(renderer);
}
} // namespace VectorVoxel::Backends
