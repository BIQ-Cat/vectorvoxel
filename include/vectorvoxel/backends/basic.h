#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <vectorvoxel/backends/export_basic.h>
#include <vectorvoxel/core/backend.h>

namespace VectorVoxel::Backends {
class VECTORVOXEL_BACKENDS_BASIC_EXPORT SimpleBackend : public Backend {
public:
  SimpleBackend() = default;

  void renderFrame(const std::vector<uint32_t> &pixels, int width,
                   int height) override {
    last_frame = pixels;
    last_width = width;
    last_height = height;

    if (on_frame)
      on_frame(pixels, width, height);
  }

  void
  setOnFrameCallback(std::function<void(const std::vector<uint32_t> &pixels,
                                        int width, int height)>
                         callback) {
    on_frame = callback;
  }

  bool isReady() override { return true; }

  std::vector<uint32_t> getLastFrame() { return last_frame; }
  void saveToBMP(std::string filename);

private:
  std::vector<uint32_t> last_frame;
  int last_width{}, last_height{};

  std::function<void(const std::vector<uint32_t> &pixels, int width,
                     int height)>
      on_frame;
};

} // namespace VectorVoxel::Backends
