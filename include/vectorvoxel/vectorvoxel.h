#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <vector>

#include <vectorvoxel/export.h>

namespace VectorVoxel {
typedef VECTORVOXEL_EXPORT
    std::function<void(int x0, int y0, int height, int color)>
        OnData;

struct VECTORVOXEL_EXPORT Camera {
  float x;
  float y;
  float z;
  double angle;
  float pitch;
  float vel;
  float angle_vel;
};

class VECTORVOXEL_EXPORT Screen {
public:
  Screen(double fov, int ray_distance, int width, int height);

  constexpr double GetFOV();
  constexpr void SetFOV(double fov);

  constexpr int GetRayDistance();
  constexpr void SetRayDistance(int ray_distance);

  constexpr int GetWidth();
  constexpr void SetWidth(int width);

  constexpr int GetHeight();
  constexpr void SetHeight(int height);

  constexpr struct Camera GetCamera();
  constexpr void MoveBy(float x_perc, float y_perc, float z_perc,
                        double angle_perc, float pitch_perc);
  constexpr void ResetCamera();
  constexpr void Move(float x, float y, float z, double angle, float pitch);

  void Render(const std::vector<std::vector<int>> &height_map,
              const std::vector<std::vector<int>> &color_map, OnData when_line);

  void RenderSync(const std::vector<std::vector<int>> &height_map,
                  const std::vector<std::vector<int>> &color_map,
                  OnData when_line);

private:
  std::atomic_int width_;
  std::atomic_int height_;
  std::atomic_int ray_distance_;

  std::mutex camera_guard_;
  double fov_;
  struct Camera camera_;

  void CastRay(int screen_x, double angle,
               const std::vector<std::vector<int>> &height_map,
               const std::vector<std::vector<int>> &color_map,
               OnData when_line);
}; // class Camera

} // namespace VectorVoxel
