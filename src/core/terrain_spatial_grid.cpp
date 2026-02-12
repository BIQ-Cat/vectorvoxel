#include <algorithm>
#include <vectorvoxel/core/terrain.h>

namespace VectorVoxel {
void TerrainMap::GridCell::add(TerrainOverlay *overlay) {

  overlays.insert(overlay);
}

void TerrainMap::GridCell::remove(TerrainOverlay *overlay) {
  overlays.erase(overlay);
}

void TerrainMap::addOverlayToGrid(TerrainOverlay *overlay) {
  if (!overlay)
    return;

  int start_gx = std::max(overlay->get_x() / grid_cell_size, 0);
  int start_gy = std::max(overlay->get_y() / grid_cell_size, 0);
  int end_gx = std::min((overlay->get_x() + overlay->width) / grid_cell_size,
                        grid_width - 1);
  int end_gy = std::min((overlay->get_y() + overlay->height) / grid_cell_size,
                        grid_height - 1);

  for (int x = start_gx; x <= end_gx; ++x) {
    for (int y = start_gy; y <= end_gy; ++y) {
      spatial_grid[y * grid_width + x].add(overlay);
    }
  }
}

void TerrainMap::removeOverlayFromGrid(TerrainOverlay *overlay) {
  if (!overlay)
    return;

  int cell_x1 = overlay->get_x() / grid_cell_size;
  int cell_y1 = overlay->get_y() / grid_cell_size;
  int cell_x2 = std::min((overlay->get_x() + overlay->width) / grid_cell_size,
                         grid_width - 1);
  int cell_y2 = std::min((overlay->get_y() + overlay->height) / grid_cell_size,
                         grid_height - 1);

  for (int y = cell_y1; y <= cell_y2; ++y) {
    for (int x = cell_x1; x <= cell_x2; ++x) {
      spatial_grid[y * grid_width + x].remove(overlay);
    }
  }
}

void TerrainMap::rebuildSpatialGrid() {
  for (auto &cell : spatial_grid) {
    cell.overlays.clear();
  }

  for (auto &overlay : overlays) {
    addOverlayToGrid(overlay.get());
  }
}

std::vector<TerrainOverlay *>
TerrainMap::getOverlapsInRegion(int x, int y, int w, int h) const {
  int start_gx = std::max(x / grid_cell_size, 0);
  int start_gy = std::max(y / grid_cell_size, 0);
  int end_gx = std::min((x + w) / grid_cell_size, grid_width - 1);
  int end_gy = std::min((y + h) / grid_cell_size, grid_height - 1);

  std::vector<TerrainOverlay *> result;

  for (int gx = start_gx; gx <= end_gx; ++gx) {
    for (int gy = start_gy; gy <= end_gy; ++gy) {
      GridCell cell = spatial_grid[gy * grid_width + gx];
      for (auto overlay : cell.overlays) {
        if (overlay->get_x() < x + w &&
            overlay->get_x() + overlay->get_width() > x &&
            overlay->get_y() < y + h &&
            overlay->get_y() + overlay->get_height() > y)
          result.push_back(overlay);
      }
    }
  }

  return result;
}

} // namespace VectorVoxel
