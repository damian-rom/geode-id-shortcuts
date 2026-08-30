#include "LabelUtils.hpp"

#include <Geode/binding/MultilineBitmapFont.hpp>
#include <Geode/binding/TextArea.hpp>

#include <algorithm>

using namespace geode::prelude;

std::vector<CCLabelBMFont*> LabelUtils::collectLabelsFrom(CCNode* node) {
    std::vector<CCLabelBMFont*> labels;
    if (!node) return labels;

    if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
        labels.push_back(label);
        return labels;
    }

    auto font = node->getChildByType<MultilineBitmapFont>(0);
    if (!font) return labels;

    auto children = font->getChildren();
    if (!children) return labels;

    for (unsigned int i = 0; i < children->count(); i++) {
        if (auto line = typeinfo_cast<CCLabelBMFont*>(children->objectAtIndex(i))) {
            labels.push_back(line);
        }
    }
    return labels;
}

std::vector<CCLabelBMFont*> LabelUtils::collectCommentLabels(CCNode* mainLayer) {
    if (!mainLayer) return {};
    if (auto n = mainLayer->getChildByIDRecursive("comment-text-label")) return collectLabelsFrom(n);
    if (auto n = mainLayer->getChildByIDRecursive("comment-text-area"))  return collectLabelsFrom(n);
    return {};
}

std::vector<CCLabelBMFont*> LabelUtils::collectDescriptionLabels(CCNode* layer) {
    if (!layer) return {};

    if (auto n = layer->getChildByIDRecursive("description-area")) return collectLabelsFrom(n);
    return {};
}

void LabelUtils::colorRange(CCLabelBMFont* label, int start, int end, ccColor3B color) {
    if (!label) return;

    for (int i = start; i < end; i++) {
        if (auto glyph = typeinfo_cast<CCSprite*>(label->getChildByTag(i))) {
            glyph->setColor(color);
        }
    }
}

CCRect LabelUtils::glyphBounds(CCLabelBMFont* label, int start, int end) {
    if (!label) return CCRectZero;

    bool found = false;
    float minX = 0.f, minY = 0.f, maxX = 0.f, maxY = 0.f;

    for (int i = start; i < end; i++) {
        auto glyph = label->getChildByTag(i);
        if (!glyph) continue;   // spaces have no glyph node

        auto box = glyph->boundingBox();

        if (!found) {
            found = true;
            minX = box.getMinX();
            minY = box.getMinY();
            maxX = box.getMaxX();
            maxY = box.getMaxY();
            continue;
        }

        minX = std::min(minX, box.getMinX());
        minY = std::min(minY, box.getMinY());
        maxX = std::max(maxX, box.getMaxX());
        maxY = std::max(maxY, box.getMaxY());
    }

    if (!found) return CCRectZero;
    return CCRect(minX, minY, maxX - minX, maxY - minY);
}
