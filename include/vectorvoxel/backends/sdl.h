#pragma once

#include "SDL2/SDL_video.h"
#include "vectorvoxel/core/backend.h"
#include <vectorvoxel/backends/export_sdl.h>
namespace VectorVoxel::Backends {
class VECTORVOXEL_BACKEND_SDL_EXPORT SDLBackend : public Backend {
public:
  SDLBackend(SDL_Window *window);
};
} // namespace VectorVoxel::Backends
