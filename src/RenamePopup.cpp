#include "RenamePopup.hpp"

RenamePopup* RenamePopup::create(std::string const& currentName, std::function<void(std::string)> callback, std::string const& title, std::string const& buttonText) {
    auto* ret = new RenamePopup();
    if (ret) {
        ret->m_currentName = currentName;
        ret->m_callback = std::move(callback);
        ret->m_title = title;
        ret->m_buttonText = buttonText;
        if (ret->init(260.0f, 140.0f)) {
            ret->autorelease();
            return ret;
        }
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool RenamePopup::init(float width, float height) {
    if (!Popup::init(width, height)) return false;

    this->setTitle(m_title.c_str());

    auto winSize = m_mainLayer->getContentSize();

    // Text input
    m_input = TextInput::create(200.0f, "Kit name...");
    m_input->setString(m_currentName);
    m_input->setPosition({winSize.width / 2.0f, winSize.height / 2.0f + 10.0f});
    m_mainLayer->addChild(m_input);

    // Submit button
    auto btnSprite = ButtonSprite::create(m_buttonText.c_str(), "goldFont.fnt", "GJ_button_01.png", 0.8f);
    auto btn = CCMenuItemSpriteExtra::create(
        btnSprite,
        this,
        menu_selector(RenamePopup::onSubmit)
    );

    auto menu = CCMenu::create();
    menu->addChild(btn);
    menu->setPosition({winSize.width / 2.0f, winSize.height / 2.0f - 35.0f});
    m_mainLayer->addChild(menu);

    return true;
}

void RenamePopup::onSubmit(CCObject* sender) {
    auto text = m_input->getString();
    if (text.empty()) {
        FLAlertLayer::create("Error", "Kit name cannot be empty.", "OK")->show();
        return;
    }
    if (m_callback) {
        m_callback(std::string(text));
    }
    this->onClose(sender);
}
