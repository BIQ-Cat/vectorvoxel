#pragma once

#include <vector>
#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
struct VECTORVOXEL_CORE_EXPORT TerrainMap {
  std::vector<std::vector<int>> height_map;
  std::vector<std::vector<int>> texture;
  int width() const { return height_map.empty() ? 0 : height_map[0].size(); };
  int height() const { return height_map.empty() ? 0 : height_map.size(); }
};
} // namespace VectorVoxel
