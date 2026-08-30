#pragma once

#include <Geode/Geode.hpp>

#include <vector>

namespace ShortcutCreator {
    int apply(
        std::vector<cocos2d::CCLabelBMFont*> const& labels,
        cocos2d::CCNode* menu,
        cocos2d::CCObject* target,
        cocos2d::SEL_MenuHandler callback,
        int skipLevelID = 0,
        std::string_view context = "?"
    );

    void handleClick(cocos2d::CCObject* sender, cocos2d::CCLayer* spinnerParent);

    void openLevel(int id, cocos2d::CCLayer* spinnerParent);

    void openList(int id, cocos2d::CCLayer* spinnerParent);
}
