#pragma once

#include <Geode/Geode.hpp>

#include <vector>

namespace LabelUtils {
    std::vector<cocos2d::CCLabelBMFont*> collectLabelsFrom(cocos2d::CCNode* node);

    std::vector<cocos2d::CCLabelBMFont*> collectCommentLabels(cocos2d::CCNode* mainLayer);

    std::vector<cocos2d::CCLabelBMFont*> collectDescriptionLabels(cocos2d::CCNode* layer);

    void colorRange(
        cocos2d::CCLabelBMFont* label,
        int start,
        int end,
        cocos2d::ccColor3B color
    );

    cocos2d::CCRect glyphBounds(cocos2d::CCLabelBMFont* label, int start, int end);
}
