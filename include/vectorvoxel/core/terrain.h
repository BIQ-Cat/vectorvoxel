#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
struct VECTORVOXEL_CORE_EXPORT TerrainMap {
  std::vector<unsigned int> height_map;
  std::vector<uint32_t> texture; // pixels in ARGB8888

  int width;
  int height;

  double scale_height_ratio;
  uint32_t max_height;

  bool should_light_up;

  bool coords_are_valid(int x, int y) const {
    return x >= 0 && y >= 0 && x < width && y < height;
  }
  int coords_to_index(int x, int y) const {
    if (coords_are_valid(x, y))
      return y * width + x;
    else
      throw std::out_of_range("x and y are not valid");
  }
};
} // namespace VectorVoxel
