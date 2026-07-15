#pragma once

#include "Kit.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

/// A small horizontal strip of SimplePlayer nodes showing a kit's icons with colors.
class IconPreview : public CCNode {
public:
    /// Create a preview for the given kit. Scale controls the size of each icon.
    static IconPreview* create(Kit const& kit, float scale = 0.45f);

    bool init(Kit const& kit, float scale);

    /// Update the preview to show a different kit.
    void updateFromKit(Kit const& kit);

private:
    float m_iconScale = 0.45f;

    void addPlayerIcon(Kit const& kit, int iconId, IconType type, float xPos);
};
