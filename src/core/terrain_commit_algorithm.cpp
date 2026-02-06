#include "vectorvoxel/core/terrain.h"
#include <vector>
namespace VectorVoxel {
std::vector<DirtyRegion> TerrainMap::collectDirtyRegions() const {
  std::vector<DirtyRegion> dirty_regions;
  for (auto &overlay : overlays) {
    const auto &dirty = overlay->getDirtyRegion();
    if (dirty.isEmpty())
      continue;

    DirtyRegion dirty_map_region{};

    int x1 = std::max(overlay->get_x() + dirty.x1, 0);
    int y1 = std::max(overlay->get_y() + dirty.y1, 0);
    int x2 = std::min(overlay->get_x() + dirty.x2, width);
    int y2 = std::min(overlay->get_y() + dirty.y2, height);

    if (x1 < x2 && y1 < y2) {
      dirty_map_region.x1 = x1;
      dirty_map_region.x2 = x2;
      dirty_map_region.y1 = y1;
      dirty_map_region.y2 = y2;

      dirty_regions.push_back(dirty_map_region);
    }
  }
  return dirty_regions;
}

std::vector<DirtyRegion>
TerrainMap::mergeDirtyRegions(const std::vector<DirtyRegion> regions) const {
  if (regions.empty())
    return {};

  std::vector<DirtyRegion> merged;
  for (const auto &region : regions) {
    if (region.isEmpty())
      continue;
    bool was_merged = false;

    for (auto &merged_region : merged) {
      if (merged_region.intersects(region)) {
        merged_region.merge(region);
        was_merged = true;
        break;
      }
    }

    if (!was_merged) {
      merged.push_back(region);
    }
  }

  return merged;
}

void TerrainMap::processDirtyRegion(const DirtyRegion &region) {
  auto overlays_in_region =
      getOverlapsInRegion(region.x1, region.y1, region.x2, region.y2);

  bool colors_changed = false;
  for (auto overlay : overlays_in_region) {
    int ox1 = std::max(region.x1 - overlay->get_x(), 0);
    int oy1 = std::max(region.y1 - overlay->get_y(), 0);
    int ox2 = std::min(overlay->width, region.x2 - overlay->get_x());
    int oy2 = std::min(overlay->height, region.y2 - overlay->get_y());

    if (ox1 >= ox2 || oy1 >= oy2)
      continue;

    for (int y = oy1; y < oy2; ++y) {
      for (int x = ox1; x < ox2; ++x) {
        int overlay_idx = y * overlay->width + x;
        int map_idx = (overlay->get_y() + y) * width + (overlay->get_x() + x);

        if (overlay->needsEnabling()) {
          height_map[map_idx] += overlay->height_map[overlay_idx];
        } else if (overlay->needsDisabling()) {
          height_map[map_idx] -= overlay->height_map[overlay_idx];
        } else if (overlay->enabled) {
          height_map[map_idx] += overlay->height_diffs[overlay_idx];
        }

        if (colors_changed) {
          overlay->underlying_colors[overlay_idx] = texture[map_idx];
        }
        if (overlay->needsDisabling() || overlay->needsColorUpdate()) {
          texture[map_idx] = overlay->underlying_colors[overlay_idx];
          if (overlay->needsColorUpdate()) {
            texture[map_idx] =
                blend(overlay->texture[overlay_idx], texture[map_idx]);
          }
          colors_changed = true;
        }
      }
    }
  }
}

void TerrainMap::removeMarkedOverlays() {
  auto it = overlays.begin();
  while (it != overlays.end()) {
    if ((*it)->isMarkedForRemoval()) {
      removeOverlayFromGrid(it->get());
      it = overlays.erase(it);
    } else {
      (*it)->clearDirtyState();
      ++it;
    }
  }
}
} // namespace VectorVoxel
