#pragma once

#include "vectorvoxel/core/terrain.h"
#include <algorithm>
#include <cstddef>
#include <memory>
namespace VectorVoxel {
TerrainMap::TerrainMap(std::vector<uint32_t> height_map,
                       std::vector<uint32_t> texture, int width, int height,
                       int scale_height_ratio, bool highlighted)
    : VoxelObject(height_map, texture, width, height),
      scale_height_ratio(scale_height_ratio), highlighted(highlighted),
      base_texture(texture) {
  max_height = *std::max_element(height_map.begin(), height_map.end());
  rebuildLUT();
}

void TerrainMap::rebuildLUT() {
  if (!highlighted)
    return;

  gray_lut.clear();
  gray_lut.reserve(max_height + 1);

  for (int h{}; h <= max_height; ++h) {
    gray_lut.push_back(h * 0xFF / max_height);
  }
}

void TerrainMap::addOverlay(std::shared_ptr<TerrainOverlay> overlay) {
  if (overlay->is_tied)
    return;

  overlay->z_index = 0;

  auto insert_pos = overlays.begin();
  while (insert_pos != overlays.end() && (*insert_pos) < overlay) {
    ++insert_pos;
  }
  overlay->id = next_id;
  ++next_id;

  overlays.insert(insert_pos, overlay);
  addOverlayToGrid(overlay.get());
  overlay->markDirtyAreaFixed(0, 0, overlay->get_width(),
                              overlay->get_height());

  int start_x = std::max(overlay->get_x(), 0);
  int start_y = std::max(overlay->get_y(), 0);
  int end_x = std::min(overlay->get_x() + overlay->get_width(), width);
  int end_y = std::min(overlay->get_y() + overlay->get_height(), height);

  if (start_x < end_x && start_y < end_y) {
    auto overlaps =
        getOverlapsInRegion(start_x, start_y, end_x - start_x, end_y - start_y);

    for (auto other : overlaps) {
      if (other > overlay.get())
        other->markDirtyAreaFixed(0, 0, overlay->get_width(),
                                  overlay->get_height());
    }
  }
}

void TerrainMap::setZIndex(std::shared_ptr<TerrainOverlay> overlay,
                           size_t z_index) {

  if (overlay->z_index == z_index || z_index < 0)
    return;

  auto pos = std::find(overlays.begin(), overlays.end(), overlay);
  if (pos == overlays.end())
    return;

  overlays.erase(pos);
  size_t old_z_index = overlay->z_index;
  overlay->z_index = z_index;

  pos = overlays.begin();
  for (; pos != overlays.end(); ++pos) {
    if ((*pos) > overlay)
      break;
  }

  removeOverlayFromGrid(overlay.get());
  overlays.insert(pos, overlay);
  addOverlayToGrid(overlay.get());
  overlay->markDirtyAreaFixed(0, 0, overlay->get_width(),
                              overlay->get_height());

  int start_x = std::max(overlay->get_x(), 0);
  int start_y = std::max(overlay->get_y(), 0);
  int end_x = std::min(overlay->get_x() + overlay->get_width(), width);
  int end_y = std::min(overlay->get_y() + overlay->get_height(), height);

  if (start_x < end_x && start_y < end_y) {
    auto overlaps =
        getOverlapsInRegion(start_x, start_y, end_x - start_x, end_y - start_y);

    for (auto other : overlaps) {
      if (!other->goesBefore(old_z_index, overlay->id))
        other->markDirtyAreaFixed(0, 0, overlay->get_width(),
                                  overlay->get_height());
    }
  }
}

void TerrainMap::commit() {
  uint32_t prev_max_height = max_height;
  for (auto &overlay : overlays) {
    if (overlay->isMarkedForRemoval() && overlay->isEnabled()) {
      overlay->disable();
    }
  }

  auto dirty_regions = collectDirtyRegions();

  auto merged_regions = mergeDirtyRegions(dirty_regions);

  for (const auto &region : merged_regions) {
    processDirtyRegion(region);
  }

  removeMarkedOverlays();
  if (max_height > prev_max_height) {
    rebuildLUT();
  }
}

} // namespace VectorVoxel
