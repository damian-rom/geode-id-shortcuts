#include <Geode/Geode.hpp>
#include <Geode/modify/GJMessagePopup.hpp>

#include <Geode/binding/GJUserMessage.hpp>
#include <Geode/binding/TextArea.hpp>

#include "../ui/ShortcutOverlay.hpp"
#include "../utils/LabelUtils.hpp"
#include "../utils/ShortcutCreator.hpp"

using namespace geode::prelude;

class $modify(IDSMessagePopup, GJMessagePopup) {
    void onShortcutClick(CCObject* sender) {
        ShortcutCreator::handleClick(sender, this);
    }

    void createMessageShortcuts(char const* stage) {
        auto body = m_mainLayer ? m_mainLayer->getChildByType<TextArea>(0) : nullptr;
        if (!body) {
            log::debug("[msg/{}] no body TextArea yet", stage);
            return;
        }

        auto menu = ShortcutOverlay::menuFor(body->getParent());
        ShortcutOverlay::clear(menu);

        if (!Mod::get()->getSettingValue<bool>("enabled")) return;

        ShortcutCreator::apply(
            LabelUtils::collectLabelsFrom(body),
            menu,
            this,
            menu_selector(IDSMessagePopup::onShortcutClick),
            0,
            fmt::format("msg/{}", stage)
        );
    }

    bool init(GJUserMessage* message) {
        if (!GJMessagePopup::init(message)) return false;
        createMessageShortcuts("init");
        return true;
    }

    void loadFromGJMessage(GJUserMessage* message) {
        GJMessagePopup::loadFromGJMessage(message);
        createMessageShortcuts("loadFromGJMessage");
    }
};
