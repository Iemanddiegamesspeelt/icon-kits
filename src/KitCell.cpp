#include "KitCell.hpp"
#include "KitManager.hpp"

KitCell* KitCell::create(Kit const& kit, size_t index, float width, KitCellCallbacks const& callbacks) {
    auto* ret = new KitCell();
    if (ret && ret->init(kit, index, width, callbacks)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool KitCell::init(Kit const& kit, size_t index, float width, KitCellCallbacks const& callbacks) {
    if (!CCNode::init()) return false;

    m_index = index;
    m_callbacks = callbacks;

    float height = 50.0f;
    this->setContentSize({width, height});
    this->setAnchorPoint({0.5f, 0.5f});

    // Background stripe (alternating)
    auto bg = CCLayerColor::create(
        index % 2 == 0
            ? ccc4(0, 0, 0, 40)
            : ccc4(255, 255, 255, 15)
    );
    bg->setContentSize({width, height});
    bg->setPosition({0, 0});
    this->addChild(bg, -1);

    // Favorite star
    CCNode* starNode = nullptr;
    auto starSprite = CCSprite::createWithSpriteFrameName("GJ_bigStar_001.png");
    if (starSprite) {
        starSprite->setScale(0.35f);
        if (!kit.favorite) {
            starSprite->setColor(ccc3(100, 100, 100));
            starSprite->setOpacity(120);
        }
        starNode = starSprite;
    } else {
        // Fallback: create a simple label star
        auto label = CCLabelBMFont::create(kit.favorite ? "*" : ".", "bigFont.fnt");
        label->setScale(0.5f);
        starNode = label;
    }
    auto starBtn = CCMenuItemSpriteExtra::create(
        starNode,
        this,
        menu_selector(KitCell::onFavorite)
    );

    // Kit name
    auto nameLabel = CCLabelBMFont::create(kit.name.c_str(), "bigFont.fnt");
    nameLabel->setAnchorPoint({0.0f, 0.5f});
    nameLabel->setPosition({54.0f, height / 2.0f + 8.0f});
    nameLabel->setScale(0.4f);
    // Truncate long names
    float maxNameWidth = width * 0.3f;
    if (nameLabel->getScaledContentSize().width > maxNameWidth) {
        nameLabel->setScale(0.4f * maxNameWidth / nameLabel->getScaledContentSize().width);
    }
    this->addChild(nameLabel);

    // Icon preview
    auto preview = IconPreview::create(kit, 0.3f);
    if (preview) {
        preview->setPosition({54.0f, height / 2.0f - 8.0f});
        preview->setAnchorPoint({0.0f, 0.5f});
        this->addChild(preview);
    }

    // Action buttons menu
    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setContentSize({width, height});

    // Star button
    starBtn->setPosition({18.0f, height / 2.0f});
    menu->addChild(starBtn);

    // Up and Down reordering buttons
    auto upSprite = CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
    auto upBtn = CCMenuItemSpriteExtra::create(
        upSprite ? static_cast<CCNode*>(upSprite) : static_cast<CCNode*>(ButtonSprite::create("^", "goldFont.fnt", "GJ_button_01.png", 0.4f)),
        this,
        menu_selector(KitCell::onMoveUp)
    );
    upBtn->setPosition({36.0f, 32.0f});
    upBtn->setScale(0.45f);

    auto downSprite = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    auto downBtn = CCMenuItemSpriteExtra::create(
        downSprite ? static_cast<CCNode*>(downSprite) : static_cast<CCNode*>(ButtonSprite::create("v", "goldFont.fnt", "GJ_button_01.png", 0.4f)),
        this,
        menu_selector(KitCell::onMoveDown)
    );
    downBtn->setPosition({36.0f, 16.0f});
    downBtn->setScale(0.45f);

    // Hide arrows if out of bounds
    size_t totalKits = KitManager::get().count();
    if (index == 0) {
        upBtn->setVisible(false);
    }
    if (index >= totalKits - 1) {
        downBtn->setVisible(false);
    }

    menu->addChild(upBtn);
    menu->addChild(downBtn);

    float btnScale = 0.45f;
    float btnX = width - 14.0f;
    float btnSpacing = 38.0f;

    // Delete button
    auto delSprite = ButtonSprite::create("Del", 30, true, "bigFont.fnt", "GJ_button_06.png", 20.0f, 0.5f);
    auto delBtn = CCMenuItemSpriteExtra::create(delSprite, this, menu_selector(KitCell::onDelete));
    delBtn->setPosition({btnX, height / 2.0f});
    delBtn->setScale(btnScale);
    menu->addChild(delBtn);
    btnX -= btnSpacing;

    // Rename button
    auto renSprite = ButtonSprite::create("Ren", 30, true, "bigFont.fnt", "GJ_button_04.png", 20.0f, 0.5f);
    auto renBtn = CCMenuItemSpriteExtra::create(renSprite, this, menu_selector(KitCell::onRename));
    renBtn->setPosition({btnX, height / 2.0f});
    renBtn->setScale(btnScale);
    menu->addChild(renBtn);
    btnX -= btnSpacing;

    // Duplicate button
    auto dupSprite = ButtonSprite::create("Dup", 30, true, "bigFont.fnt", "GJ_button_04.png", 20.0f, 0.5f);
    auto dupBtn = CCMenuItemSpriteExtra::create(dupSprite, this, menu_selector(KitCell::onDuplicate));
    dupBtn->setPosition({btnX, height / 2.0f});
    dupBtn->setScale(btnScale);
    menu->addChild(dupBtn);
    btnX -= btnSpacing;

    // Save button
    auto saveSprite = ButtonSprite::create("Save", 30, true, "bigFont.fnt", "GJ_button_02.png", 20.0f, 0.5f);
    auto saveBtn = CCMenuItemSpriteExtra::create(saveSprite, this, menu_selector(KitCell::onSave));
    saveBtn->setPosition({btnX, height / 2.0f});
    saveBtn->setScale(btnScale);
    menu->addChild(saveBtn);
    btnX -= btnSpacing;

    // Load button
    auto loadSprite = ButtonSprite::create("Load", 35, true, "bigFont.fnt", "GJ_button_01.png", 20.0f, 0.5f);
    auto loadBtn = CCMenuItemSpriteExtra::create(loadSprite, this, menu_selector(KitCell::onLoad));
    loadBtn->setPosition({btnX, height / 2.0f});
    loadBtn->setScale(btnScale);
    menu->addChild(loadBtn);

    this->addChild(menu);

    return true;
}

void KitCell::onLoad(CCObject* sender) {
    if (m_callbacks.onLoad) m_callbacks.onLoad(m_index);
}

void KitCell::onSave(CCObject* sender) {
    if (m_callbacks.onSave) m_callbacks.onSave(m_index);
}

void KitCell::onDuplicate(CCObject* sender) {
    if (m_callbacks.onDuplicate) m_callbacks.onDuplicate(m_index);
}

void KitCell::onRename(CCObject* sender) {
    if (m_callbacks.onRename) m_callbacks.onRename(m_index);
}

void KitCell::onDelete(CCObject* sender) {
    if (m_callbacks.onDelete) m_callbacks.onDelete(m_index);
}

void KitCell::onFavorite(CCObject* sender) {
    if (m_callbacks.onToggleFavorite) m_callbacks.onToggleFavorite(m_index);
}

void KitCell::onExport(CCObject* sender) {
    if (m_callbacks.onExport) m_callbacks.onExport(m_index);
}

void KitCell::onMoveUp(CCObject* sender) {
    if (m_callbacks.onMove) m_callbacks.onMove(m_index, true);
}

void KitCell::onMoveDown(CCObject* sender) {
    if (m_callbacks.onMove) m_callbacks.onMove(m_index, false);
}
