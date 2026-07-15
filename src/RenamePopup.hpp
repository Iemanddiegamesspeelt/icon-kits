#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <functional>

using namespace geode::prelude;

/// A small popup with a text input field for renaming kits.
class RenamePopup : public Popup {
protected:
    TextInput* m_input = nullptr;
    std::string m_currentName;
    std::string m_title;
    std::string m_buttonText;
    std::function<void(std::string)> m_callback;

    bool init(float width, float height);

    void onSubmit(CCObject* sender);

public:
    static RenamePopup* create(std::string const& currentName, std::function<void(std::string)> callback, std::string const& title = "Rename Kit", std::string const& buttonText = "Rename");
};
