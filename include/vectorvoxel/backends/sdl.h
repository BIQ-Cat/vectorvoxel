#pragma once

#include "SDL2/SDL.h"
#include "vectorvoxel/core/backend.h"
#include <vectorvoxel/backends/export_sdl.h>
namespace VectorVoxel::Backends {
class VECTORVOXEL_BACKEND_SDL_EXPORT SDLBackend : public Backend {
public:
  SDLBackend(SDL_Window *window, bool vsync = true);
  ~SDLBackend();

  SDLBackend(const SDLBackend &) = delete;
  SDLBackend &operator=(const SDLBackend &) = delete;

  void renderFrame(const std::vector<uint32_t> &pixels, int width,
                   int height) override;
  bool isReady() override;

  SDL_Renderer *getSDLRenderer() const { return renderer; }
  SDL_Texture *getSDLTexture() const { return texture; }

#ifdef __EMSCRIPTEN__
  void setCanvasId(const std::string &canvas_id);
#endif
private:
  SDL_Window *window;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;

  int last_width = 0;
  int last_height = 0;
  bool use_vsync;

  void restoreTexture(int width, int height);
  void cleanup();
};
} // namespace VectorVoxel::Backends
