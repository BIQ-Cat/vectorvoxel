#include <cstdint>
#include <vectorvoxel/core/terrain.h>

namespace VectorVoxel {
uint32_t blend(uint32_t upper, uint32_t lower) {
  uint32_t alpha = (upper >> 24) & 0xFF;

  // Fast paths
  if (alpha == 0xFF)
    return upper;
  if (alpha == 0x00)
    return lower;

  uint32_t inv_alpha = 255 - alpha;

  // Extract channels
  uint32_t ur = (upper >> 16) & 0xFF;
  uint32_t ug = (upper >> 8) & 0xFF;
  uint32_t ub = upper & 0xFF;

  uint32_t lr = (lower >> 16) & 0xFF;
  uint32_t lg = (lower >> 8) & 0xFF;
  uint32_t lb = lower & 0xFF;

  // Blend using integer arithmetic
  uint32_t r = (ur * alpha + lr * inv_alpha) / 255;
  uint32_t g = (ug * alpha + lg * inv_alpha) / 255;
  uint32_t b = (ub * alpha + lb * inv_alpha) / 255;

  return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

} // namespace VectorVoxel
