#pragma once

#include "vectorvoxel/core/voxel.h"
#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <vectorvoxel/core/export.h>

namespace VectorVoxel {
uint32_t blend(uint32_t upper, uint32_t lower);
struct VECTORVOXEL_CORE_EXPORT DirtyRegion {
  int x1 = INT_MAX, y1 = INT_MAX;
  int x2 = -1, y2 = -1;

  void expand(int x, int y) {
    x1 = std::min(x1, x);
    y1 = std::min(y1, y);
    x2 = std::max(x2, x + 1);
    y2 = std::max(y2, y + 1);
  }

  void expand(int x, int y, int w, int h) {
    x1 = std::min(x1, x);
    y1 = std::min(y1, y);
    x2 = std::max(x2, x + w);
    y2 = std::max(y2, y + h);
  }

  bool intersects(const DirtyRegion &other) const {
    return std::max(x1, other.x1) < std::min(x2, other.x2) &&
           std::max(y1, other.y1) < std::min(y2, other.y2);
  }

  void merge(const DirtyRegion &other) {
    expand(other.x1, other.y1, other.width(), other.height());
  }

  int width() const { return x2 - x1; }
  int height() const { return y2 - y1; }
  bool isEmpty() const { return x2 <= x1 || y2 <= y1; }
};

class VECTORVOXEL_CORE_EXPORT TerrainOverlay : public VoxelObject {
public:
  TerrainOverlay(std::vector<uint32_t> height_map,
                 std::vector<uint32_t> texture, int width, int height, int x,
                 int y, bool is_enabled);

  void modifyHeight(int x, int y, uint32_t height);
  void modifyColor(int x, int y, uint32_t color);
  void blendColor(int x, int y, uint32_t color);
  void applyColorFilter(int x, int y, uint32_t color_mask);

  void disable();
  void enable();
  bool isEnabled() const { return enabled; }

  void rejectChanges();

  int get_x() const { return x; }
  int get_y() const { return y; }

  void markForRemoval();

  bool isMarkedForRemoval() const { return marked_for_removal; }
  bool needsColorUpdate() const {
    return needsEnabling() || enabled && colors_changed;
  }
  bool needsEnabling() const { return enabled && !original_enabled; }
  bool needsDisabling() const { return !enabled && original_enabled; }

  DirtyRegion getDirtyRegion() const {
    DirtyRegion res;
    res.merge(dirty_region);
    res.merge(fixed_dirty_region);
    return res;
  }
  const std::vector<int64_t> &getHeightDifferences() const {
    return height_diffs;
  }

  bool operator<(const TerrainOverlay &other) {
    return goesBefore(other.z_index, other.id);
  }
  bool operator>(const TerrainOverlay &other) {
    return !goesBefore(other.z_index, other.id);
  }

private:
  friend class TerrainMap;
  int x;
  int y;

  size_t z_index = SIZE_MAX;
  size_t id = SIZE_MAX;
  bool goesBefore(size_t z_index, size_t id) {
    return this->z_index < z_index ||
           (this->z_index == z_index && this->id < id);
  }

  bool enabled = true;
  bool marked_for_removal = false;
  bool is_tied = false;

  bool colors_changed = false;
  std::vector<int64_t> height_diffs;

  DirtyRegion dirty_region;
  DirtyRegion fixed_dirty_region;
  void markDirty(int x, int y);
  void markDirtyArea(int x, int y, int w, int h);
  void markDirtyFixed(int x, int y);
  void markDirtyAreaFixed(int x, int y, int w, int h);

  void clearDirtyState();

  std::vector<uint32_t> original_height_map;
  std::vector<uint32_t> original_texture;
  bool original_enabled;

  std::vector<uint32_t> underlying_colors;
};

class VECTORVOXEL_CORE_EXPORT TerrainMap : public VoxelObject {
public:
  TerrainMap(std::vector<uint32_t> height_map, std::vector<uint32_t> texture,
             int width, int height, int scale_height_ratio, bool highlighted);

  void addOverlay(std::shared_ptr<TerrainOverlay> overlay);
  std::vector<std::shared_ptr<TerrainOverlay>> getOverlays() const {
    return std::vector(overlays.begin(), overlays.end());
  }

  size_t getZIndex(std::shared_ptr<TerrainOverlay> overlay) const {
    return overlay->z_index;
  };
  void setZIndex(std::shared_ptr<TerrainOverlay> overlay, size_t z_index);

  void moveOverlay(std::shared_ptr<TerrainOverlay> overlay, int dx, int dy) {};

  void commit();

  uint32_t getMaxHeight() const { return max_height; }
  double scaleHeight(int height) const { return height * scale_height_ratio; }

  bool isHighlighted() const { return highlighted; }
  std::optional<uint8_t> getGrayTypeOf(uint32_t height) const {
    if (height > max_height)
      return std::nullopt;

    return gray_lut[height];
  }

private:
  std::vector<uint32_t> base_texture;

  int scale_height_ratio;
  bool highlighted;

  std::vector<uint32_t> gray_lut;
  void rebuildLUT();

  uint32_t max_height;
  size_t next_id = 1;

  std::list<std::shared_ptr<TerrainOverlay>> overlays;

  struct GridCell {
    std::set<TerrainOverlay *> overlays;

    void add(TerrainOverlay *overlay);
    void remove(TerrainOverlay *overlay);
  };

  std::vector<GridCell> spatial_grid;
  int grid_width, grid_height;
  int grid_cell_size = 32;

  void addOverlayToGrid(TerrainOverlay *overlay);
  void removeOverlayFromGrid(TerrainOverlay *overlay);
  void rebuildSpatialGrid();

  std::vector<TerrainOverlay *> getOverlapsInRegion(int x, int y, int w,
                                                    int h) const;

  std::vector<DirtyRegion> collectDirtyRegions() const;

  std::vector<DirtyRegion>
  mergeDirtyRegions(const std::vector<DirtyRegion> regions) const;

  void processDirtyRegion(const DirtyRegion &region);

  void removeMarkedOverlays();
};

} // namespace VectorVoxel
