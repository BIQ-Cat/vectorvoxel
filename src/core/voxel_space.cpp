#include "thread_pool.h"
#include "vectorvoxel/core/backend.h"
#include "vectorvoxel/core/renderer.h"
#include "vectorvoxel/core/terrain.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace VectorVoxel {
class Renderer::Impl {
public:
  std::vector<std::shared_ptr<Backend>> backends;
  int screen_width, screen_height;
  int ray_distance;

  std::unique_ptr<ThreadPool> thread_pool;
  std::size_t num_threads;

  std::shared_ptr<TerrainMap> terrain;

  void initializeThreadPool() {
    auto num_threads = std::max(1u, std::thread::hardware_concurrency());

#ifdef __EMSCRIPTEN__
    num_threads = std::min(4u, num_threads);
#endif

    thread_pool = std::make_unique<ThreadPool>(std::max(1u, num_threads - 1));
  }

  void castRay(double ray_angle, const Camera &camera, uint32_t *buffer_start) {
    double sin = std::sin(ray_angle);
    double cos = std::cos(ray_angle);

    int smallest_y = screen_height;

    for (double i = 1.0; i < ray_distance; ++i) {
      int y = i * sin + camera.position.y;
      int x = i * cos + camera.position.x;

      if (!terrain->coords_are_valid(x, y)) {
        continue;
      }

      double normal = i * std::cos(camera.orientation.yaw - ray_angle);

      uint32_t pixel_height = terrain->get_height_of(x, y);
      uint32_t pixel_color = terrain->get_color_of(x, y);
      int height =
          (terrain->scaleHeight(camera.position.z - pixel_height) / normal) +
          camera.orientation.pitch_ox;

      height = std::max(height, 0);

      if (height < smallest_y) {
        for (int screen_y = height; screen_y < smallest_y; ++screen_y) {
          uint32_t color = pixel_color;
          if (!terrain->isHighlighted()) {
            uint8_t gray = terrain->getGrayTypeOf(pixel_height).value();

            uint8_t a = 0xFF;
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 0) & 0xFF;

            r = ((r + gray) >> 1) & 0xFF;
            g = ((g + gray) >> 1) & 0xFF;
            b = ((b + gray) >> 1) & 0xFF;

            color = (a << 24) | (r << 16) | (g << 8) | (b << 0);
          }
          *(buffer_start + screen_y * screen_width) = color;
        }
      }
    }
  }
};

Renderer::Renderer(Viewport viewport)
    : viewport(viewport), impl(std::make_unique<Impl>()) {
  setViewport(viewport);
}

void Renderer::setViewport(Viewport viewport) {
  this->viewport = viewport;
  impl->screen_height = viewport.height;
  impl->screen_width = viewport.width;
  impl->ray_distance = viewport.ray_distance;
}

void Renderer::setTerrainMap(std::shared_ptr<TerrainMap> terrain) {
  setTerrainMap(terrain, terrain->height_max());
}

void Renderer::setTerrainMap(std::shared_ptr<TerrainMap> terrain,
                             uint32_t height_cap) {
  impl->terrain = terrain;
}

void Renderer::addBackend(std::shared_ptr<Backend> backend) {
  if (backend && backend->isReady()) {
    impl->backends.push_back(backend);
  }
}

void Renderer::removeBackend(std::shared_ptr<Backend> backend) {
  impl->backends.erase(
      std::remove(impl->backends.begin(), impl->backends.end(), backend),
      impl->backends.end());
}

void Renderer::clearBackends() { impl->backends.clear(); }

const std::vector<std::shared_ptr<Backend>> &Renderer::getBackends() {
  return impl->backends;
}

std::vector<uint32_t> Renderer::renderToBuffer(const Camera &camera) {
  auto size = viewport.width * viewport.height;
  std::vector<uint32_t> pixels(size, 0xFF000000);

  static std::once_flag init_pool_flag;
  std::call_once(init_pool_flag, [this]() { impl->initializeThreadPool(); });

  if (impl->terrain) {
    double start_angle = camera.orientation.yaw - (viewport.fov_rad / 2);
    double delta_angle = viewport.fov_rad / viewport.width;

    std::vector<double> ray_angles(viewport.width);
    for (int x{}; x < viewport.width; ++x) {
      ray_angles[x] = start_angle + delta_angle * x;
    }

    int chunk_size = std::max(1, viewport.width / (int)impl->num_threads / 2);
    for (int chunk_start{}; chunk_start < viewport.width;
         chunk_start += chunk_size) {
      int chunk_end = std::min(chunk_start + chunk_size, viewport.width);
      impl->thread_pool->enqueue(
          [this, chunk_start, chunk_end, &camera, &ray_angles, &pixels]() {
            for (int x = chunk_start; x < chunk_end; ++x) {
              impl->castRay(ray_angles[x], camera, &pixels[x]);
            }
          });
    }

    impl->thread_pool->wait_all();
  }
  return pixels;
}

void Renderer::renderFrame(const Camera &camera) {
  std::vector<uint32_t> frame = renderToBuffer(camera);
  last_frame = frame;

  for (auto &backend : impl->backends) {
    if (backend->isReady()) {
      backend->renderFrame(frame, viewport.width, viewport.height);
    }
  }
}
} // namespace VectorVoxel
