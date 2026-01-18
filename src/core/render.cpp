#include <cmath>
#include <thread>
#include <vector>
#include <vectorvoxel/vectorvoxel.h>

void VectorVoxel::Screen::Render(const std::vector<std::vector<int>> &height_map,
                                 const std::vector<std::vector<int>> &color_map, OnData when_line)
{
    double delta_angle = fov_ / width_;
    double angle = camera_.angle - fov_ / 2;
    std::vector<std::thread> threads(this->width_);
    for (int x{}; x < width_; ++x)
    {
        threads[x] = std::thread([x, angle, &height_map, &color_map, &when_line, this] {
            this->CastRay(x, angle, height_map, color_map, when_line);
        });
        angle += delta_angle;
    }

    for (int i{}; i < width_; ++i)
    {
        threads[i].join();
    }
}

void VectorVoxel::Screen::RenderSync(const std::vector<std::vector<int>> &height_map,
                                     const std::vector<std::vector<int>> &color_map, VectorVoxel::OnData when_line)
{
    double delta_angle = fov_ / width_;
    double angle = camera_.angle - fov_ / 2;
    for (int x{}; x < width_; ++x)
    {
        CastRay(x, angle, height_map, color_map, when_line);
        angle += delta_angle;
    }
}

void VectorVoxel::Screen::CastRay(int screen_x, double angle, const std::vector<std::vector<int>> &height_map,
                                  const std::vector<std::vector<int>> &color_map, VectorVoxel::OnData when_line)
{

    double sin = std::sin(angle);
    double cos = std::cos(angle);

    int smallest_y = height_;

    for (int d{1}; d < ray_distance_; ++d)
    {
        int z = d * sin + camera_.z;
        if (z < 0 || z >= height_map.size())
        {
            continue;
        }
        int x = d * cos + camera_.x;
        if (x < 0 || x >= height_map[0].size())
        {
            continue;
        }

        double depth = z * std::cos(camera_.angle - angle);

        int y = height_map[z][y];
        int height_on_screen = (camera_.y - y) / depth * 360 + camera_.pitch;

        if (height_on_screen < smallest_y)
        {
            when_line(screen_x, smallest_y, smallest_y - height_on_screen, color_map[z][y]);
            smallest_y = height_on_screen;
        }
    }
}
