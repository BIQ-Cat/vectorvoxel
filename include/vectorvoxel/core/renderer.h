#pragma once

#include <cstdint>
#include <memory>

#include <vectorvoxel/core/backend.h>
#include <vectorvoxel/core/camera.h>
#include <vectorvoxel/core/terrain.h>

#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
class VECTORVOXEL_CORE_EXPORT Renderer {
public:
  Renderer(const Viewport viewport);
  ~Renderer();

  void renderFrame(const Camera &camera);
  std::vector<uint32_t> renderToBuffer(const Camera &camera);

  void addBackend(std::shared_ptr<Backend> backend);
  void removeBackend(std::shared_ptr<Backend> backend);
  void clearBackends();
  const std::vector<std::shared_ptr<Backend>> &getBackends();

  void setTerrainMap(std::shared_ptr<TerrainMap> terrain);
  void setTerrainMap(std::shared_ptr<TerrainMap> terrain, uint32_t height_cap);

  std::vector<uint32_t> getLastBuffer() const { return last_frame; };
  Viewport getViewport() { return viewport; };
  void setViewport(Viewport viewport);

private:
  Viewport viewport;
  std::vector<uint32_t> last_frame;

  class Impl;
  std::unique_ptr<Impl> impl;
};
} // namespace VectorVoxel
