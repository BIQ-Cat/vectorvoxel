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
  Viewport &viewport;
  Renderer(const Viewport &viewport);
  ~Renderer();

  void renderFrame(const TerrainMap &terrain, const Camera &camera);
  std::vector<uint32_t> renderToBuffer(const TerrainMap &terrain,
                                       const Camera &camera);

  void addBackend(std::shared_ptr<Backend> backend);
  void removeBackend(std::shared_ptr<Backend> backend);
  void clearBackends();
  const std::vector<std::shared_ptr<Backend>> &getBackends();

  std::vector<uint32_t> getLastBuffer();

private:
  std::vector<uint32_t> renderVoxelSpace(const TerrainMap &terrain,
                                         const Camera &Camera);

  class Impl;
  std::unique_ptr<Impl> impl;
};
} // namespace VectorVoxel
