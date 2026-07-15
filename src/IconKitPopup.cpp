#include "IconKitPopup.hpp"
#include "RenamePopup.hpp"
#include "ImportExport.hpp"
#include "Utils.hpp"
#include <Geode/binding/GJGarageLayer.hpp>

static GJGarageLayer* findGarageLayer(CCNode* parent) {
    if (!parent) return nullptr;
    if (auto garage = typeinfo_cast<GJGarageLayer*>(parent)) {
        return garage;
    }
    auto children = parent->getChildren();
    if (children) {
        for (int i = 0; i < children->count(); ++i) {
            auto child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (auto found = findGarageLayer(child)) {
                return found;
            }
        }
    }
    return nullptr;
}

IconKitPopup* IconKitPopup::create() {
    auto* ret = new IconKitPopup();
    if (ret && ret->init(440.0f, 310.0f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool IconKitPopup::init(float width, float height) {
    if (!Popup::init(width, height)) return false;

    this->setTitle("Icon Kits");

    auto winSize = m_mainLayer->getContentSize();

    // Search bar at the top
    m_searchInput = TextInput::create(300.0f, "Search kits...");
    m_searchInput->setPosition({winSize.width / 2.0f, winSize.height - 45.0f});
    m_searchInput->setCallback([this](std::string const& text) {
        this->onSearchChanged(text);
    });
    m_mainLayer->addChild(m_searchInput);

    // Kit count label
    m_countLabel = CCLabelBMFont::create("0 kits", "chatFont.fnt");
    m_countLabel->setPosition({winSize.width - 55.0f, winSize.height - 45.0f});
    m_countLabel->setScale(0.5f);
    m_countLabel->setColor(ccc3(200, 200, 200));
    m_mainLayer->addChild(m_countLabel);

    // Scroll layer for the kit list
    float scrollWidth = 400.0f;
    float scrollHeight = 175.0f;
    m_scrollLayer = ScrollLayer::create({scrollWidth, scrollHeight});
    m_scrollLayer->setPosition({
        (winSize.width - scrollWidth) / 2.0f,
        65.0f
    });
    m_mainLayer->addChild(m_scrollLayer);

    // Border around scroll area
    auto border = CCLayerColor::create(ccc4(255, 255, 255, 20));
    border->setContentSize({scrollWidth + 4.0f, scrollHeight + 4.0f});
    border->setPosition({
        (winSize.width - scrollWidth) / 2.0f - 2.0f,
        63.0f
    });
    m_mainLayer->addChild(border, -1);

    // Bottom button bar - Row 1
    auto bottomMenu1 = CCMenu::create();
    bottomMenu1->setPosition({winSize.width / 2.0f, 46.0f});
    bottomMenu1->setLayout(
        RowLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Center)
    );

    // Bottom button bar - Row 2
    auto bottomMenu2 = CCMenu::create();
    bottomMenu2->setPosition({winSize.width / 2.0f, 24.0f});
    bottomMenu2->setLayout(
        RowLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Center)
    );

    // New button
    auto newSprite = ButtonSprite::create("New", "goldFont.fnt", "GJ_button_01.png", 0.7f);
    auto newBtn = CCMenuItemSpriteExtra::create(newSprite, this, menu_selector(IconKitPopup::onNew));
    newBtn->setScale(0.7f);
    bottomMenu1->addChild(newBtn);

    // Save Current button
    auto saveSprite = ButtonSprite::create("Save Current", "goldFont.fnt", "GJ_button_01.png", 0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(saveSprite, this, menu_selector(IconKitPopup::onSaveCurrent));
    saveBtn->setScale(0.7f);
    bottomMenu1->addChild(saveBtn);

    // Random button
    auto randomSprite = ButtonSprite::create("Random", "goldFont.fnt", "GJ_button_05.png", 0.7f);
    auto randomBtn = CCMenuItemSpriteExtra::create(randomSprite, this, menu_selector(IconKitPopup::onRandom));
    randomBtn->setScale(0.7f);
    bottomMenu1->addChild(randomBtn);

    // Sort button
    auto sortSprite = ButtonSprite::create("Sort", "goldFont.fnt", "GJ_button_05.png", 0.7f);
    auto sortBtn = CCMenuItemSpriteExtra::create(sortSprite, this, menu_selector(IconKitPopup::onSort));
    sortBtn->setScale(0.7f);
    bottomMenu2->addChild(sortBtn);

    // Import button
    auto importSprite = ButtonSprite::create("Import", "goldFont.fnt", "GJ_button_04.png", 0.7f);
    auto importBtn = CCMenuItemSpriteExtra::create(importSprite, this, menu_selector(IconKitPopup::onImport));
    importBtn->setScale(0.7f);
    bottomMenu2->addChild(importBtn);

    // Export All button
    auto exportSprite = ButtonSprite::create("Export", "goldFont.fnt", "GJ_button_04.png", 0.7f);
    auto exportBtn = CCMenuItemSpriteExtra::create(exportSprite, this, menu_selector(IconKitPopup::onExport));
    exportBtn->setScale(0.7f);
    bottomMenu2->addChild(exportBtn);

    bottomMenu1->updateLayout();
    bottomMenu2->updateLayout();
    m_mainLayer->addChild(bottomMenu1);
    m_mainLayer->addChild(bottomMenu2);

    // Sort mode label
    m_sortLabel = CCLabelBMFont::create(
        KitManager::get().getSortModeName().c_str(),
        "chatFont.fnt"
    );
    m_sortLabel->setPosition({winSize.width / 2.0f, 9.0f});
    m_sortLabel->setScale(0.4f);
    m_sortLabel->setColor(ccc3(180, 180, 180));
    m_mainLayer->addChild(m_sortLabel);

    // Build the list
    refreshList();

    return true;
}

std::vector<size_t> IconKitPopup::getFilteredIndices() {
    std::string query = m_searchInput ? std::string(m_searchInput->getString()) : "";
    return KitManager::get().search(query);
}

void IconKitPopup::refreshList() {
    if (!m_scrollLayer) return;

    // Clear existing content
    m_scrollLayer->m_contentLayer->removeAllChildren();

    auto indices = getFilteredIndices();
    auto const& allKits = KitManager::get().getKits();

    float cellWidth = m_scrollLayer->getContentSize().width;
    float cellHeight = 50.0f;

    KitCellCallbacks callbacks;
    callbacks.onLoad = [this](size_t idx) { this->onKitLoad(idx); };
    callbacks.onSave = [this](size_t idx) { this->onKitSave(idx); };
    callbacks.onDuplicate = [this](size_t idx) { this->onKitDuplicate(idx); };
    callbacks.onRename = [this](size_t idx) { this->onKitRename(idx); };
    callbacks.onDelete = [this](size_t idx) { this->onKitDelete(idx); };
    callbacks.onToggleFavorite = [this](size_t idx) { this->onKitToggleFavorite(idx); };
    callbacks.onExport = [this](size_t idx) { this->onKitExport(idx); };
    callbacks.onMove = [this](size_t idx, bool up) {
        KitManager::get().moveKit(idx, up);
        this->refreshList();
    };

    for (size_t i = 0; i < indices.size(); ++i) {
        size_t kitIndex = indices[i];
        if (kitIndex >= allKits.size()) continue;

        auto cell = KitCell::create(allKits[kitIndex], kitIndex, cellWidth, callbacks);
        if (cell) {
            m_scrollLayer->m_contentLayer->addChild(cell);
        }
    }

    // Layout
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setGap(0.0f)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
    );
    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->m_contentLayer->setPositionY(
        m_scrollLayer->getContentHeight() - m_scrollLayer->m_contentLayer->getContentHeight()
    );

    // Update count label
    if (m_countLabel) {
        auto text = fmt::format("{} kit{}", indices.size(), indices.size() == 1 ? "" : "s");
        m_countLabel->setString(text.c_str());
    }

    // Update sort label
    if (m_sortLabel) {
        m_sortLabel->setString(KitManager::get().getSortModeName().c_str());
    }
}

void IconKitPopup::onSearchChanged(std::string const& text) {
    refreshList();
}

void IconKitPopup::onNew(CCObject* sender) {
    RenamePopup::create("New Kit", [this](std::string name) {
        KitManager::get().createKit(name);
        this->refreshList();
    })->show();
}

void IconKitPopup::onSaveCurrent(CCObject* sender) {
    RenamePopup::create("My Kit", [this](std::string name) {
        KitManager::get().createKit(name);
        this->refreshList();
        KitsUtils::showInfo("Saved", "Current icons saved as \"" + name + "\"!");
    })->show();
}

void IconKitPopup::onImport(CCObject* sender) {
    ImportExport::importFromClipboard();
    refreshList();
}

void IconKitPopup::onExport(CCObject* sender) {
    ImportExport::exportAllToClipboard();
}

void IconKitPopup::onRandom(CCObject* sender) {
    int idx = KitManager::get().getRandomKitIndex();
    if (idx < 0) {
        KitsUtils::showError("Random Kit", "No kits available. Create some first!");
        return;
    }
    KitManager::get().loadKit(static_cast<size_t>(idx));
    auto kit = KitManager::get().getKit(static_cast<size_t>(idx));
    std::string name = kit ? kit->name : "Unknown";

    // Close the popup first
    this->onClose(nullptr);

    // Refresh the GJGarageLayer behind the popup
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene) {
        if (auto garage = findGarageLayer(scene)) {
            auto gm = GameManager::get();
            if (gm && garage->m_playerObject) {
                auto player = garage->m_playerObject;
                player->updatePlayerFrame(gm->getPlayerFrame(), IconType::Cube);
                player->updatePlayerFrame(gm->getPlayerShip(), IconType::Ship);
                player->updatePlayerFrame(gm->getPlayerBall(), IconType::Ball);
                player->updatePlayerFrame(gm->getPlayerBird(), IconType::Ufo);
                player->updatePlayerFrame(gm->getPlayerDart(), IconType::Wave);
                player->updatePlayerFrame(gm->getPlayerRobot(), IconType::Robot);
                player->updatePlayerFrame(gm->getPlayerSpider(), IconType::Spider);
                player->updatePlayerFrame(gm->getPlayerSwing(), IconType::Swing);
                player->updatePlayerFrame(gm->getPlayerJetpack(), IconType::Jetpack);
                
                int activeIconId = 1;
                switch (garage->m_selectedIconType) {
                    case IconType::Cube: activeIconId = gm->getPlayerFrame(); break;
                    case IconType::Ship: activeIconId = gm->getPlayerShip(); break;
                    case IconType::Ball: activeIconId = gm->getPlayerBall(); break;
                    case IconType::Ufo: activeIconId = gm->getPlayerBird(); break;
                    case IconType::Wave: activeIconId = gm->getPlayerDart(); break;
                    case IconType::Robot: activeIconId = gm->getPlayerRobot(); break;
                    case IconType::Spider: activeIconId = gm->getPlayerSpider(); break;
                    case IconType::Swing: activeIconId = gm->getPlayerSwing(); break;
                    case IconType::Jetpack: activeIconId = gm->getPlayerJetpack(); break;
                }
                player->updatePlayerFrame(activeIconId, garage->m_selectedIconType);
            }
            garage->selectTab(garage->m_selectedIconType);
            garage->updatePlayerColors();
        }
    }

    KitsUtils::showInfo("Random Kit", "Applied \"" + name + "\"!");
}

void IconKitPopup::onSort(CCObject* sender) {
    KitManager::get().cycleSort();
    refreshList();
}

void IconKitPopup::onKitLoad(size_t index) {
    KitManager::get().loadKit(index);
    auto kit = KitManager::get().getKit(index);
    std::string name = kit ? kit->name : "Kit";

    // Close the popup first
    this->onClose(nullptr);

    // Refresh the GJGarageLayer behind the popup so it displays the new icons
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene) {
        if (auto garage = findGarageLayer(scene)) {
            auto gm = GameManager::get();
            if (gm && garage->m_playerObject) {
                auto player = garage->m_playerObject;
                player->updatePlayerFrame(gm->getPlayerFrame(), IconType::Cube);
                player->updatePlayerFrame(gm->getPlayerShip(), IconType::Ship);
                player->updatePlayerFrame(gm->getPlayerBall(), IconType::Ball);
                player->updatePlayerFrame(gm->getPlayerBird(), IconType::Ufo);
                player->updatePlayerFrame(gm->getPlayerDart(), IconType::Wave);
                player->updatePlayerFrame(gm->getPlayerRobot(), IconType::Robot);
                player->updatePlayerFrame(gm->getPlayerSpider(), IconType::Spider);
                player->updatePlayerFrame(gm->getPlayerSwing(), IconType::Swing);
                player->updatePlayerFrame(gm->getPlayerJetpack(), IconType::Jetpack);
                
                int activeIconId = 1;
                switch (garage->m_selectedIconType) {
                    case IconType::Cube: activeIconId = gm->getPlayerFrame(); break;
                    case IconType::Ship: activeIconId = gm->getPlayerShip(); break;
                    case IconType::Ball: activeIconId = gm->getPlayerBall(); break;
                    case IconType::Ufo: activeIconId = gm->getPlayerBird(); break;
                    case IconType::Wave: activeIconId = gm->getPlayerDart(); break;
                    case IconType::Robot: activeIconId = gm->getPlayerRobot(); break;
                    case IconType::Spider: activeIconId = gm->getPlayerSpider(); break;
                    case IconType::Swing: activeIconId = gm->getPlayerSwing(); break;
                    case IconType::Jetpack: activeIconId = gm->getPlayerJetpack(); break;
                }
                player->updatePlayerFrame(activeIconId, garage->m_selectedIconType);
            }
            garage->selectTab(garage->m_selectedIconType);
            garage->updatePlayerColors();
        }
    }

    KitsUtils::showInfo("Loaded", "Applied \"" + name + "\"!");
}

void IconKitPopup::onKitSave(size_t index) {
    auto kit = KitManager::get().getKit(index);
    if (!kit) return;

    geode::createQuickPopup(
        "Update Kit",
        "Do you want to overwrite \"" + kit->name + "\" with your current icons?",
        "Cancel",
        "Update",
        [this, index](FLAlertLayer* popup, bool btn2) {
            if (btn2) {
                KitManager::get().updateKit(index);
                this->refreshList();
                KitsUtils::showInfo("Updated", "Kit updated successfully!");
            }
        }
    );
}

void IconKitPopup::onKitDuplicate(size_t index) {
    KitManager::get().duplicateKit(index);
    refreshList();
}

void IconKitPopup::onKitRename(size_t index) {
    auto kit = KitManager::get().getKit(index);
    if (!kit) return;

    RenamePopup::create(kit->name, [this, index](std::string newName) {
        KitManager::get().renameKit(index, newName);
        this->refreshList();
    })->show();
}

void IconKitPopup::onKitDelete(size_t index) {
    auto kit = KitManager::get().getKit(index);
    if (!kit) return;

    KitManager::get().deleteKit(index);
    refreshList();
}

void IconKitPopup::onKitToggleFavorite(size_t index) {
    KitManager::get().toggleFavorite(index);
    refreshList();
}

void IconKitPopup::onKitExport(size_t index) {
    ImportExport::exportKitToClipboard(index);
}
