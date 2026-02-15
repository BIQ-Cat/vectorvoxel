#pragma once

#include <math.h>
#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
struct VECTORVOXEL_CORE_EXPORT Viewport {
  int width;
  int height;
  double fov_rad = M_PI / 2;
  int ray_distance = 1000;

  Viewport(int w, int h, double fov = M_PI / 2, int ray_dist = 1000)
      : width(w), height(h), fov_rad(fov), ray_distance(ray_dist) {}
};

struct VECTORVOXEL_CORE_EXPORT Camera {
  struct Position {
    int x{}, y{}, z{150};
  } position;

  struct Orientation {
    double yaw{M_PI / 4};
    int pitch_ox = -10;
  } orientation;

  struct Movement {
    float velocity = 5.0f;
    float angle_velocity = 1.5f;
  } movement;

  void moveForward(float perc_speed);
  void strafe(float perc_speed);
  void rotate(float perc_speed_yaw, float perc_speed_pitch);
  void fly(float perc_speed) { position.z += movement.velocity * perc_speed; };
};
} // namespace VectorVoxel
