#include "vectorvoxel/core/file_headers.h"
#include "vectorvoxel/core/voxel.h"
#include <cstdint>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <vector>

inline uint32_t parseColor(uint8_t *color_start, uint8_t color_depth,
                           uint8_t alpha_depth) {
  uint8_t no_alpha_depth = color_depth - alpha_depth;
  uint8_t component_depth = no_alpha_depth / 3;

  uint32_t data{};
  for (int i{}, n{}; i < color_depth; i += 8, ++n) {
    data |= *(color_start + n) << i;
  }

  uint32_t result{};

  uint8_t max_component_color = (1 << component_depth) - 1;
  uint8_t component_ratio = 255 / max_component_color;

  result |= (data & max_component_color) * component_ratio;
  result |= ((data >> component_depth) & max_component_color) * component_ratio;
  result |=
      ((data >> component_depth >> component_depth) & max_component_color) *
      component_ratio;

  if (alpha_depth == 0) {
    result |= 0xFF << 24;
  } else {
    result |= ((data >> no_alpha_depth) * 1 << (8 - alpha_depth)) << 24;
  }

  return result;
}

namespace VectorVoxel {
std::vector<uint32_t> loadTextureFromTGA(std::string filename) {
  std::ifstream tga_file(filename, std::ios::binary);
  TGAHeader header;
  tga_file.read(reinterpret_cast<char *>(&header), sizeof(header));
  tga_file.ignore(header.id_length);

  int color_size = header.color_depth / 8;
  std::vector<uint8_t> color_map(header.color_map_length * color_size);

  int pixel_size = header.pixel_depth / 8;
  int image_row_size = header.image_width * pixel_size;
  std::vector<uint8_t> image(image_row_size * header.image_height);

  tga_file.read(reinterpret_cast<char *>(color_map.data()), color_map.size());
  tga_file.read(reinterpret_cast<char *>(image.data()), image.size());

  std::vector<uint8_t *> rows(header.image_height);
  for (int y{}; y < header.image_height; ++y) {
    rows[y] = image.data() + y * image_row_size;
  }

  if (~header.image_descriptor & (1 << 5)) {
    std::reverse(rows.begin(), rows.end());
  }

  if (header.image_descriptor & (1 << 4)) {
    for (int y{}; y < header.image_height; ++y) {
      uint8_t *start = rows[y];
      uint8_t *end = start + image_row_size;

      std::reverse(start, end);
    }
  }

  std::vector<uint32_t> pixels(header.image_width * header.image_height);
  switch (header.image_type) {
  case 0: // No image
    break;
  case 1: // Color map
    if (!header.color_map_type)
      throw std::runtime_error("Malformed tga file");
    for (int y{}; y < header.image_height; ++y) {
      for (int x{}; x < header.image_width; ++x) {
        uint8_t *pixel_ptr = rows[y] + x * pixel_size;
        size_t index;
        if (header.pixel_depth == 32) {
          index = static_cast<size_t>(*reinterpret_cast<uint32_t *>(pixel_ptr));
        } else if (header.pixel_depth == 16) {
          index = static_cast<size_t>(*reinterpret_cast<uint16_t *>(pixel_ptr));
        } else if (header.pixel_depth == 8) {
          index = static_cast<size_t>(*reinterpret_cast<uint8_t *>(pixel_ptr));
        }

        index -= header.first_color_index;

        uint8_t *color_ptr = &color_map[index * color_size];
        pixels[y * header.image_width + x] =
            parseColor(color_ptr, header.color_depth,
                       header.image_descriptor & ((1 << 4) - 1));
      }
    }

    break;
  case 2: // True color
    for (int y{}; y < header.image_height; ++y) {
      for (int x{}; x < header.image_width; ++x) {
        uint8_t *color_ptr = rows[y] + x * pixel_size;
        pixels[y * header.image_width + x] =
            parseColor(color_ptr, header.pixel_depth,
                       header.image_descriptor & ((1 << 4) - 1));
      }
    }

    break;
  case 3: // Grayscale
    if (pixel_size != 1 && pixel_size != 2) {
      throw std::runtime_error("Unsupported TGA");
    }
    for (int y{}; y < header.image_height; ++y) {
      for (int x{}; x < header.image_width; ++x) {
        uint8_t *pixel_ptr = rows[y] + x * pixel_size;
        uint32_t color = (*pixel_ptr << 16) | (*pixel_ptr << 8) | (*pixel_ptr);
        if (pixel_size == 2) {
          color |= *(pixel_ptr + 1) << 24;
        } else {
          color |= 0xFF << 24;
        }

        pixels[y * header.image_width + x] = color;
      }
    }
    break;
  default:
    throw std::runtime_error("Unsupported TGA");
  }

  return pixels;
}
} // namespace VectorVoxel
