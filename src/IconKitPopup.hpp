#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include "KitManager.hpp"
#include "KitCell.hpp"

using namespace geode::prelude;

/// The main popup for managing icon kits.
class IconKitPopup : public Popup {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_searchInput = nullptr;
    CCLabelBMFont* m_sortLabel = nullptr;
    CCLabelBMFont* m_countLabel = nullptr;

    bool init(float width, float height);

    /// Rebuild the entire kit list from KitManager.
    void refreshList();

    /// Get the filtered list of kit indices based on search.
    std::vector<size_t> getFilteredIndices();

    // Button callbacks
    void onNew(CCObject* sender);
    void onSaveCurrent(CCObject* sender);
    void onImport(CCObject* sender);
    void onExport(CCObject* sender);
    void onRandom(CCObject* sender);
    void onSort(CCObject* sender);

    // KitCell callbacks
    void onKitLoad(size_t index);
    void onKitSave(size_t index);
    void onKitDuplicate(size_t index);
    void onKitRename(size_t index);
    void onKitDelete(size_t index);
    void onKitToggleFavorite(size_t index);
    void onKitExport(size_t index);

    // Search callback
    void onSearchChanged(std::string const& text);

public:
    static IconKitPopup* create();
};
