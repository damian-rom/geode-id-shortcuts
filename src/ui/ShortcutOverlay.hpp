#pragma once

#include <Geode/Geode.hpp>

namespace ShortcutOverlay {
    std::string idPrefix();

    void clear(cocos2d::CCNode* menu);

    CCMenuItemSpriteExtra* add(
        cocos2d::CCNode* menu,
        cocos2d::CCLabelBMFont* label,
        int start,
        int end,
        int id,
        int index,
        cocos2d::CCObject* target,
        cocos2d::SEL_MenuHandler callback
    );

    cocos2d::CCMenu* menuFor(cocos2d::CCNode* parent);

    int idOf(cocos2d::CCObject* sender);

    bool openModifierHeld();
}
