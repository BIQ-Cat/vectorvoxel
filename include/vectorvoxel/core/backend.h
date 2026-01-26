#pragma once

#include <cstdint>
#include <vector>
#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
class VECTORVOXEL_CORE_EXPORT Backend {
public:
  virtual ~Backend() = default;

  virtual void renderFrame(const std::vector<uint32_t> &pixels, int width,
                           int height) = 0;
  virtual bool isReady() { return true; }
};
} // namespace VectorVoxel
