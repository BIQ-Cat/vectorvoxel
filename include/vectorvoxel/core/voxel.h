#pragma once

#include "vectorvoxel/core/export.h"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace VectorVoxel {
class VECTORVOXEL_CORE_EXPORT VoxelObject {
public:
  VoxelObject(std::vector<uint32_t> height_map, std::vector<uint32_t> texture,
              int width, int height)
      : height_map(height_map), texture(texture), width(width),
        height(height) {};

  bool coords_are_valid(int x, int y) const {
    return x >= 0 && y >= 0 && x < width && y < height;
  }

  uint32_t get_height_of(int x, int y) const {
    if (coords_are_valid(x, y))
      return height_map[y * width + x];
    else
      throw std::out_of_range("x and y are not valid");
  }

  uint32_t get_height_of(int index) const { return height_map[index]; }

  uint32_t get_color_of(int x, int y) const {
    if (coords_are_valid(x, y))
      return texture[y * width + x];
    else
      throw std::out_of_range("x and y are not valid");
  }
  uint32_t get_color_of(int index) const { return texture[index]; }

  int get_width() const { return width; }
  int get_height() const { return height; }
  int height_max() {
    return *(std::max_element(height_map.cbegin(), height_map.cend()));
  }

protected:
  std::vector<uint32_t> height_map;
  std::vector<uint32_t> texture;

  int width;
  int height;
};

std::vector<uint32_t> loadTextureFromTGA(std::string filename);
// std::vector<uint32_t> loadHeightMapFromHMP(std::string filename);
// std::vector<uint32_t> loadHeightMapFromBMP(std::string filename);
} // namespace VectorVoxel
