#include <Geode/Geode.hpp>
#include <Geode/modify/InfoLayer.hpp>

#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/TextArea.hpp>

#include "../ui/ShortcutOverlay.hpp"
#include "../utils/LabelUtils.hpp"
#include "../utils/ShortcutCreator.hpp"

using namespace geode::prelude;

class $modify(IDSInfoLayer, InfoLayer) {
    void onShortcutClick(CCObject* sender) {
        ShortcutCreator::handleClick(sender, this);
    }
    
    void createDescriptionShortcuts(char const* stage) {
        auto descNode = this->getChildByIDRecursive("description-area");
        if (!descNode) {
            log::debug("[desc/{}] no description node (level lists and profiles have none)",
                stage);
            return;
        }

        auto menu = ShortcutOverlay::menuFor(descNode->getParent());
        ShortcutOverlay::clear(menu);

        if (!Mod::get()->getSettingValue<bool>("enabled")) return;

        ShortcutCreator::apply(
            LabelUtils::collectLabelsFrom(descNode),
            menu,
            this,
            menu_selector(IDSInfoLayer::onShortcutClick),
            m_level ? m_level->m_levelID.value() : 0,
            fmt::format("desc/{}", stage)
        );
    }

    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
        if (!InfoLayer::init(level, score, list)) return false;
        createDescriptionShortcuts("init");
        return true;
    }

    void loadPage(int page, bool noSetup) {
        InfoLayer::loadPage(page, noSetup);
        createDescriptionShortcuts("loadPage");
    }
};
