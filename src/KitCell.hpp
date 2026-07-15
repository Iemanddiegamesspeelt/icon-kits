#pragma once

#include "Kit.hpp"
#include "IconPreview.hpp"
#include <Geode/Geode.hpp>
#include <functional>

using namespace geode::prelude;

/// Callback types for KitCell actions.
struct KitCellCallbacks {
    std::function<void(size_t)> onLoad;
    std::function<void(size_t)> onSave;
    std::function<void(size_t)> onDuplicate;
    std::function<void(size_t)> onRename;
    std::function<void(size_t)> onDelete;
    std::function<void(size_t)> onToggleFavorite;
    std::function<void(size_t)> onExport;
    std::function<void(size_t, bool)> onMove;
};

/// A single row in the kit list showing the kit name, preview, and action buttons.
class KitCell : public CCNode {
public:
    static KitCell* create(Kit const& kit, size_t index, float width, KitCellCallbacks const& callbacks);

    bool init(Kit const& kit, size_t index, float width, KitCellCallbacks const& callbacks);

private:
    size_t m_index = 0;
    KitCellCallbacks m_callbacks;

    void onLoad(CCObject* sender);
    void onSave(CCObject* sender);
    void onDuplicate(CCObject* sender);
    void onRename(CCObject* sender);
    void onDelete(CCObject* sender);
    void onFavorite(CCObject* sender);
    void onExport(CCObject* sender);
    void onMoveUp(CCObject* sender);
    void onMoveDown(CCObject* sender);
};
