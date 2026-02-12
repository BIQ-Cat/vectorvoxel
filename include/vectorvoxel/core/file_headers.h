#pragma once

#include <cstdint>
namespace VectorVoxel {
#pragma pack(push, 1)
struct TGAHeader {
  uint8_t id_length;
  uint8_t color_map_type;
  uint8_t image_type; // Compressed files are not supported

  // Color map specification
  uint16_t first_color_index;
  uint16_t color_map_length;
  uint8_t color_depth;

  // Image specification
  uint16_t x_origin;
  uint16_t y_origin;
  uint16_t image_width;
  uint16_t image_height;
  uint8_t pixel_depth;
  uint8_t image_descriptor;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HMPHeader {
  uint16_t width;
  uint16_t height;
  uint8_t depth;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NTBMPFileHeader {
  uint16_t signature = 0x4D42; // BM
  uint32_t file_size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
};
struct NTBMPInfoHeader {
  uint32_t header_size;
  int32_t bitmap_width;
  int32_t bitmap_height;
  uint16_t color_planes = 1;
  uint16_t bits_per_pixel;
  uint32_t compression = 0;           // Compressed files are not supported
  uint32_t image_size = 0;            // Compressed files are not supported
  int32_t horizontal_resolution = 0;  // Why this is even a thing?
  int32_t vertical_resolution = 0;    // Why this is even a thing?
  uint32_t palette_colors_number = 0; // Palette is not supported
  uint32_t palette_colors_number_important = 0; // Palette is not supported
};
#pragma pack(pop)

} // namespace VectorVoxel
