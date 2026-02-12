#include "vectorvoxel/core/file_headers.h"
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <vector>
#include <vectorvoxel/backends/basic.h>

namespace VectorVoxel::Backends {
void SimpleBackend::saveToBMP(std::string filename) {
  int row_size = last_width * 4;
  int data_size = row_size * last_height;
  std::vector<uint8_t> data(data_size);
  for (int y{}; y < last_height; ++y) {
    for (int x{}; x < last_width; ++x) {
      uint32_t color = last_frame[y * last_width + x];
      uint8_t a = (color >> 24) & 0xFF;
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = (color >> 0) & 0xFF;

      int offset = (last_height - 1 - y) * row_size + x * 4;
      data[offset + 0] = b;
      data[offset + 1] = g;
      data[offset + 2] = r;
      data[offset + 3] = a;
    }
  }

  NTBMPFileHeader file_header = {};
  file_header.file_size =
      sizeof(NTBMPFileHeader) + sizeof(NTBMPInfoHeader) + data_size;
  file_header.offset = sizeof(NTBMPFileHeader) + sizeof(NTBMPInfoHeader);

  NTBMPInfoHeader info_header = {};
  info_header.bits_per_pixel = 32;
  info_header.header_size = sizeof(NTBMPInfoHeader);
  info_header.bitmap_width = last_width;
  info_header.bitmap_height = last_height;

  std::ofstream file(filename, std::ios::binary);
  file.write(reinterpret_cast<char *>(&file_header), sizeof(file_header));
  file.write(reinterpret_cast<char *>(&info_header), sizeof(info_header));
  file.write(reinterpret_cast<char *>(data.data()), data_size);
}
} // namespace VectorVoxel::Backends
