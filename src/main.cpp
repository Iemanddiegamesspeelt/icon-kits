#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include "IconKitPopup.hpp"
#include "KitManager.hpp"
#include "RenamePopup.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

static CCNode* findNodeByID(CCNode* parent, std::string const& id) {
    if (!parent) return nullptr;
    if (parent->getID() == id) return parent;
    auto children = parent->getChildren();
    if (children) {
        for (int i = 0; i < children->count(); ++i) {
            auto child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (auto found = findNodeByID(child, id)) {
                return found;
            }
        }
    }
    return nullptr;
}

/// Hook into GJGarageLayer to add the "Kits" button and load saved kits.
class $modify(IconKitsGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        // Load kits from disk on first garage open
        static bool loaded = false;
        if (!loaded) {
            KitManager::get().loadFromDisk();
            loaded = true;
        }

        // Create the "Kits" button
        auto btnSprite = ButtonSprite::create(
            "Kits",
            "goldFont.fnt",
            "GJ_button_01.png",
            0.8f
        );
        btnSprite->setScale(0.9f);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(IconKitsGarageLayer::onIconKits)
        );

        // Position the button in the top-right area of the garage
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Create a dedicated menu for our button
        auto menu = CCMenu::create();
        menu->setPosition({winSize.width - 60.0f, winSize.height - 170.0f});
        menu->setID("icon-kits-menu"_spr);
        this->addChild(menu, 10);

        btn->setPosition({0, 0});
        menu->addChild(btn);

        return true;
    }

    void onIconKits(CCObject* sender) {
        auto popup = IconKitPopup::create();
        if (popup) {
            popup->show();
        }
    }
};

/// Hook into ProfilePage to allow copying another user's kit.
class $modify(IconKitsProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        if (!score) return;

        // Try to find the left-menu with fallbacks
        CCNode* leftMenuNode = this->getChildByID("left-menu");
        if (!leftMenuNode && m_mainLayer) {
            leftMenuNode = m_mainLayer->getChildByID("left-menu");
        }
        if (!leftMenuNode) {
            leftMenuNode = findNodeByID(this, "left-menu");
        }

        if (auto leftMenu = typeinfo_cast<CCMenu*>(leftMenuNode)) {
            if (leftMenu->getChildByID("copy-kit-btn"_spr)) return;

            auto copySprite = CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
            if (!copySprite) {
                copySprite = CCSprite::createWithSpriteFrameName("GJ_copyBtn_001.png");
            }
            if (!copySprite) {
                copySprite = ButtonSprite::create("Kit", "goldFont.fnt", "GJ_button_01.png", 0.6f);
            }

            auto btn = CCMenuItemSpriteExtra::create(
                copySprite,
                this,
                menu_selector(IconKitsProfilePage::onCopyKit)
            );
            btn->setID("copy-kit-btn"_spr);

            leftMenu->addChild(btn);
            leftMenu->updateLayout();
        }
    }

    void onCopyKit(CCObject* sender) {
        auto score = this->m_score;
        if (!score) return;

        int cube = score->m_playerCube;
        int ship = score->m_playerShip;
        int ball = score->m_playerBall;
        int ufo = score->m_playerUfo;
        int wave = score->m_playerWave;
        int robot = score->m_playerRobot;
        int spider = score->m_playerSpider;
        int swing = score->m_playerSwing;
        int jetpack = score->m_playerJetpack;
        int primaryColor = score->m_color1;
        int secondaryColor = score->m_color2;
        int glowColor = score->m_color3;
        bool glow = score->m_glowEnabled;
        int deathEffect = score->m_playerExplosion;
        int streak = score->m_playerStreak;

        std::string defaultName = score->m_userName + "'s Kit";
        
        RenamePopup::create(
            defaultName,
            [cube, ship, ball, ufo, wave, robot, spider, swing, jetpack, 
             primaryColor, secondaryColor, glowColor, glow, deathEffect, streak](std::string name) {
                Kit kit;
                kit.name = name.empty() ? "Copied Kit" : name;
                
                kit.cube = cube;
                kit.ship = ship;
                kit.ball = ball;
                kit.ufo = ufo;
                kit.wave = wave;
                kit.robot = robot;
                kit.spider = spider;
                kit.swing = swing;
                kit.jetpack = jetpack;
                
                kit.primaryColor = primaryColor;
                kit.secondaryColor = secondaryColor;
                kit.glowColor = glowColor;
                kit.glow = glow;
                
                kit.deathEffect = deathEffect;
                kit.streak = streak;
                kit.shipFire = 1;
                kit.createdAt = KitsUtils::currentTimestamp();

                KitManager::get().addKit(kit);
                KitsUtils::showInfo("Copied Kit", "Successfully copied \"" + kit.name + "\"!");
            },
            "Copy Kit",
            "Copy"
        )->show();
    }
};
