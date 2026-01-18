#include <cmath>
#include <vectorvoxel/vectorvoxel.h>

VectorVoxel::Screen::Screen(double fov, int ray_distance, int width, int height)
{
    fov_ = fov;
    ray_distance_.store(ray_distance);
    width_.store(width);
    height_.store(height);

    ResetCamera();
}

constexpr struct VectorVoxel::Camera VectorVoxel::Screen::GetCamera()
{
    return camera_;
}

constexpr void VectorVoxel::Screen::ResetCamera()
{
    camera_.x = 0;
    camera_.y = 150;
    camera_.z = 0;
    camera_.angle = M_PI / 4;
    camera_.pitch = -10;
    camera_.vel = 2;
    camera_.angle_vel = 0.02;
}

constexpr void VectorVoxel::Screen::Move(float x, float y, float z, double angle, float pitch)
{
    camera_.x = x;
    camera_.y = y;
    camera_.z = z;
    camera_.angle = angle;
    camera_.pitch = pitch;
}

constexpr void VectorVoxel::Screen::MoveBy(float x_perc, float y_perc, float z_perc, double angle_perc,
                                           float pitch_perc)
{
    double sin = std::sin(camera_.angle);
    double cos = std::cos(camera_.angle);

    camera_.x += x_perc * cos * camera_.vel;
    camera_.z += x_perc * sin * camera_.vel;

    camera_.x += z_perc * sin * camera_.vel;
    camera_.z -= z_perc * cos * camera_.vel;

    camera_.angle += angle_perc * camera_.angle_vel;
    camera_.pitch += pitch_perc * camera_.vel * 2;

    camera_.y += y_perc * camera_.vel;
}

constexpr int VectorVoxel::Screen::GetWidth()
{
    return width_.load();
}

constexpr void VectorVoxel::Screen::SetWidth(int width)
{
    if (width > 0)
    {
        width_.store(width);
    }
}

constexpr int VectorVoxel::Screen::GetHeight()
{
    return height_.load();
}

constexpr void VectorVoxel::Screen::SetHeight(int height)
{
    if (height > 0)
    {
        height_.store(height);
    }
}

constexpr int VectorVoxel::Screen::GetRayDistance()
{
    return ray_distance_.load();
}

constexpr void VectorVoxel::Screen::SetRayDistance(int ray_distance)
{
    if (ray_distance > 0)
    {
        ray_distance_.store(ray_distance);
    }
}

constexpr double VectorVoxel::Screen::GetFOV()
{
    return fov_;
}

constexpr void VectorVoxel::Screen::SetFOV(double fov)
{
    fov_ = fov;
}
