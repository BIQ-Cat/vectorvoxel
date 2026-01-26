#include <cmath>
#include <vectorvoxel/core/camera.h>

void VectorVoxel::Camera::moveForward(float perc_speed) {
  double sin = std::sin(orientation.yaw);
  double cos = std::cos(orientation.yaw);

  position.x += perc_speed * movement.velocity * cos;
  position.y += perc_speed * movement.velocity * sin;
}

void VectorVoxel::Camera::rotate(float perc_speed_yaw, float perc_speed_pitch) {
  orientation.yaw = movement.angle_velocity * perc_speed_yaw;
  orientation.pitch_ox = movement.velocity * perc_speed_pitch * 2;
}

void VectorVoxel::Camera::strafe(float perc_speed) {
  double sin = std::sin(orientation.yaw);
  double cos = std::cos(orientation.yaw);

  position.x += perc_speed * movement.velocity * sin;
  position.y -= perc_speed * movement.velocity * cos;
}
