#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <vector>
#include <vectorvoxel/backends/basic.h>

#ifdef __EMSCRIPTEN__
#define PACKED_STRUCT __attribute__((packed))
#elif defined(_MSC_VER)
#define PACKED_STRUCT __pragma(pack(push, 1)) struct __pragma(pack(pop))
#else
#define PACKED_STRUCT __attribute__((packed))
#endif

struct PACKED_STRUCT BMPFileHeader {
  uint16_t signature = 0x4D42; // BM
  uint32_t fileSize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t dataOffset;
};

struct PACKED_STRUCT BMPInfoHeader {
  uint32_t headerSize;
  int32_t width;
  int32_t height;
  uint16_t planes = 1;
  uint16_t bitsPerPixel = 32;
  uint32_t compression = 0;
  uint32_t imageSize = 0;
  int32_t xPixelsPerMeter = 0;
  int32_t yPixelsPerMeter = 0;
  uint32_t colorsUsed = 0;
  uint32_t colorsImportant = 0;
};

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

  BMPFileHeader fileHeader = {};
  fileHeader.fileSize =
      sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + data_size;
  fileHeader.dataOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

  BMPInfoHeader infoHeader = {};
  infoHeader.headerSize = sizeof(BMPInfoHeader);
  infoHeader.width = last_width;
  infoHeader.height = last_height;

  std::ofstream file(filename, std::ios::binary);
  file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
  file.write(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));
  file.write(reinterpret_cast<char *>(data.data()), data_size);
}
} // namespace VectorVoxel::Backends
