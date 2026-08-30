#include "ShortcutOverlay.hpp"
#include "../utils/LabelUtils.hpp"

#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>

#include <vector>

using namespace geode::prelude;

namespace {
#ifdef GEODE_IS_MOBILE // Bigger hitbox on mobile because why not
    constexpr float VERTICAL_PADDING = 8.f;
    constexpr float HORIZONTAL_PADDING = 4.f;
#else
    constexpr float VERTICAL_PADDING = 3.f;
    constexpr float HORIZONTAL_PADDING = 0.f;
#endif
}

std::string ShortcutOverlay::idPrefix() {
    static const std::string prefix = "shortcut-"_spr;
    return prefix;
}

void ShortcutOverlay::clear(CCNode* menu) {
    if (!menu) return;

    auto children = menu->getChildren();
    if (!children) return;

    auto const& prefix = idPrefix();

    std::vector<CCNode*> stale;
    for (unsigned int i = 0; i < children->count(); i++) {
        auto child = static_cast<CCNode*>(children->objectAtIndex(i));
        if (!child) continue;

        std::string id = child->getID();
        if (id.rfind(prefix, 0) == 0) stale.push_back(child);
    }

    for (auto node : stale) node->removeFromParent();
}

CCMenuItemSpriteExtra* ShortcutOverlay::add(
    CCNode* menu,
    CCLabelBMFont* label,
    int start,
    int end,
    int id,
    int index,
    CCObject* target,
    SEL_MenuHandler callback
) {
    if (!menu || !label) return nullptr;

    auto rect = LabelUtils::glyphBounds(label, start, end);
    if (rect.size.width <= 0.f || rect.size.height <= 0.f) return nullptr;

    auto worldBL = label->convertToWorldSpace({ rect.getMinX(), rect.getMinY() });
    auto worldTR = label->convertToWorldSpace({ rect.getMaxX(), rect.getMaxY() });
    auto localBL = menu->convertToNodeSpace(worldBL);
    auto localTR = menu->convertToNodeSpace(worldTR);

    float width = (localTR.x - localBL.x) + HORIZONTAL_PADDING * 2.f;
    float height = (localTR.y - localBL.y) + VERTICAL_PADDING * 2.f;
    if (width <= 0.f || height <= 0.f) return nullptr;

    bool debug = Mod::get()->getSettingValue<bool>("debug-mode");
    auto hitArea = CCLayerColor::create(
        debug ? ccColor4B { 255, 0, 0, 90 } : ccColor4B { 0, 0, 0, 0 },
        width, height
    );
    if (!hitArea) return nullptr;

    auto button = CCMenuItemSpriteExtra::create(hitArea, target, callback);
    if (!button) return nullptr;

    button->setPosition({
        (localBL.x + localTR.x) / 2.f,
        (localBL.y + localTR.y) / 2.f
    });

    button->setSizeMult(1.f);
    button->setID(fmt::format("{}{}", idPrefix(), index));
    button->setUserObject(CCInteger::create(id));

    menu->addChild(button);
    return button;
}

CCMenu* ShortcutOverlay::menuFor(CCNode* parent) {
    if (!parent) return nullptr;

    if (auto existing = typeinfo_cast<CCMenu*>(parent->getChildByID("shortcut-menu"_spr))) {
        return existing;
    }

    auto menu = CCMenu::create();
    if (!menu) return nullptr;

    menu->ignoreAnchorPointForPosition(false);
    menu->setAnchorPoint({ 0.f, 0.f });
    menu->setPosition({ 0.f, 0.f });
    menu->setContentSize(parent->getContentSize());
    menu->setZOrder(100);
    menu->setID("shortcut-menu"_spr);

    parent->addChild(menu);
    return menu;
}

int ShortcutOverlay::idOf(CCObject* sender) {
    auto node = typeinfo_cast<CCNode*>(sender);
    if (!node) return 0;

    auto value = typeinfo_cast<CCInteger*>(node->getUserObject());
    if (!value) return 0;

    return value->getValue();
}

bool ShortcutOverlay::openModifierHeld() {
    auto dispatcher = CCDirector::sharedDirector()->getKeyboardDispatcher();
    if (!dispatcher) return false;

    return dispatcher->getControlKeyPressed() || dispatcher->getCommandKeyPressed();
}
