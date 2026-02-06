#include <vectorvoxel/core/terrain.h>

namespace VectorVoxel {
TerrainOverlay::TerrainOverlay(std::vector<uint32_t> height_map,
                               std::vector<uint32_t> texture, int width,
                               int height, int x, int y, bool is_enabled)
    : VoxelObject(height_map, texture, width, height), x(x), y(y),
      enabled(is_enabled), original_height_map(height_map),
      original_texture(texture), height_diffs(width * height, 0),
      underlying_colors(width * height, 0) {};

void TerrainOverlay::modifyHeight(int x, int y, uint32_t height) {
  if (!coords_are_valid(x, y))
    return;

  int idx = y * width + x;
  height_diffs[idx] = static_cast<int64_t>(height) -
                      static_cast<int64_t>(original_height_map[idx]);

  height_map[idx] = height;
  markDirty(x, y);
}

void TerrainOverlay::modifyColor(int x, int y, uint32_t color) {
  if (!coords_are_valid(x, y))
    return;

  colors_changed = true;

  int idx = y * width + x;
  texture[idx] = color;
  markDirty(x, y);
}

void TerrainOverlay::blendColor(int x, int y, uint32_t color) {
  if (!coords_are_valid(x, y))
    return;

  colors_changed = true;
  int idx = y * width + x;
  texture[idx] = blend(color, original_texture[idx]);
  markDirty(x, y);
}

void TerrainOverlay::applyColorFilter(int x, int y, uint32_t color_mask) {
  if (!coords_are_valid(x, y))
    return;

  colors_changed = true;
  int idx = y * width + x;
  texture[idx] = original_texture[idx] & color_mask;

  markDirty(x, y);
}

void TerrainOverlay::disable() {
  if (!enabled)
    return;

  enabled = false;

  for (int i = 0; i < width * height; ++i) {
    height_diffs[i] = -static_cast<int64_t>(height_map[i]);
  }

  markDirtyArea(0, 0, width, height);
}

void TerrainOverlay::enable() {
  if (enabled)
    return;

  enabled = true;

  for (int i = 0; i < width * height; ++i) {
    height_diffs[i] = static_cast<int64_t>(height_map[i]) -
                      static_cast<int64_t>(original_height_map[i]);
  }

  markDirtyArea(0, 0, width, height);
}

void TerrainOverlay::markForRemoval() {
  marked_for_removal = true;
  disable();
}

void TerrainOverlay::rejectChanges() {
  if (dirty_region.isEmpty())
    return;

  for (int y = dirty_region.y1; y < dirty_region.y2; ++y) {
    for (int x = dirty_region.x1; x < dirty_region.x2; ++x) {
      int idx = y * width + x;
      height_diffs[idx] = 0;
      height_map[idx] = original_height_map[idx];
      texture[idx] = original_texture[idx];
    }
  }

  // Cannot change DirtyRegion: there can be unrejectable changes from
  // TerrainMap
  colors_changed = false;
  marked_for_removal = false;
  enabled = original_enabled;
}

void TerrainOverlay::clearDirtyState() {
  if (dirty_region.isEmpty())
    return;

  for (int y = dirty_region.y1; y < dirty_region.y2; ++y) {
    for (int x = dirty_region.x1; x < dirty_region.x2; ++x) {
      int idx = y * width + x;
      height_diffs[idx] = 0;
      original_height_map[idx] = height_map[idx];
      original_texture[idx] = texture[idx];
    }
  }

  dirty_region = DirtyRegion{};
  colors_changed = false;
  original_enabled = enabled;
}

void TerrainOverlay::markDirty(int x, int y) { dirty_region.expand(x, y); }

void TerrainOverlay::markDirtyArea(int x, int y, int w, int h) {
  dirty_region.expand(x, y, w, h);
}
} // namespace VectorVoxel
