#include "IconPreview.hpp"

IconPreview* IconPreview::create(Kit const& kit, float scale) {
    auto* ret = new IconPreview();
    if (ret && ret->init(kit, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool IconPreview::init(Kit const& kit, float scale) {
    if (!CCNode::init()) return false;
    m_iconScale = scale;

    // We show 7 icons in a row: cube, ship, ball, ufo, wave, robot, spider
    float spacing = 22.0f * scale / 0.45f;
    float totalWidth = spacing * 6.0f;
    this->setContentSize({totalWidth + 20.0f, 30.0f * scale / 0.45f});
    this->setAnchorPoint({0.5f, 0.5f});

    float startX = 10.0f;
    addPlayerIcon(kit, kit.cube, IconType::Cube, startX);
    addPlayerIcon(kit, kit.ship, IconType::Ship, startX + spacing);
    addPlayerIcon(kit, kit.ball, IconType::Ball, startX + spacing * 2);
    addPlayerIcon(kit, kit.ufo, IconType::Ufo, startX + spacing * 3);
    addPlayerIcon(kit, kit.wave, IconType::Wave, startX + spacing * 4);
    addPlayerIcon(kit, kit.robot, IconType::Robot, startX + spacing * 5);
    addPlayerIcon(kit, kit.spider, IconType::Spider, startX + spacing * 6);

    return true;
}

void IconPreview::addPlayerIcon(Kit const& kit, int iconId, IconType type, float xPos) {
    auto* gm = GameManager::get();
    if (!gm) return;

    auto* player = SimplePlayer::create(0);
    if (!player) return;

    player->updatePlayerFrame(iconId, type);
    player->setColor(gm->colorForIdx(kit.primaryColor));
    player->setSecondColor(gm->colorForIdx(kit.secondaryColor));
    if (kit.glow) {
        player->setGlowOutline(gm->colorForIdx(kit.glowColor));
    }

    player->setScale(m_iconScale);
    player->setPosition({xPos, this->getContentSize().height / 2.0f});
    this->addChild(player);
}

void IconPreview::updateFromKit(Kit const& kit) {
    this->removeAllChildren();

    float spacing = 22.0f * m_iconScale / 0.45f;
    float startX = 10.0f;
    addPlayerIcon(kit, kit.cube, IconType::Cube, startX);
    addPlayerIcon(kit, kit.ship, IconType::Ship, startX + spacing);
    addPlayerIcon(kit, kit.ball, IconType::Ball, startX + spacing * 2);
    addPlayerIcon(kit, kit.ufo, IconType::Ufo, startX + spacing * 3);
    addPlayerIcon(kit, kit.wave, IconType::Wave, startX + spacing * 4);
    addPlayerIcon(kit, kit.robot, IconType::Robot, startX + spacing * 5);
    addPlayerIcon(kit, kit.spider, IconType::Spider, startX + spacing * 6);
}
