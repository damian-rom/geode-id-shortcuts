#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>

#include "../ui/ShortcutOverlay.hpp"
#include "../utils/LabelUtils.hpp"
#include "../utils/ShortcutCreator.hpp"

using namespace geode::prelude;

class $modify(IDSCommentCell, CommentCell) {
    CCLayer* enclosingLayer() {
        for (auto node = this->getParent(); node; node = node->getParent()) {
            if (auto layer = typeinfo_cast<CCLayer*>(node)) {
                if (node->getID() != "content-layer") return layer;
            }
        }
        return nullptr;
    }

    void onShortcutClick(CCObject* sender) {
        ShortcutCreator::handleClick(sender, enclosingLayer());
    }

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        if (!comment) return;

        auto menu = m_mainLayer ? m_mainLayer->getChildByID("main-menu") : nullptr;

        ShortcutOverlay::clear(menu);

        if (!Mod::get()->getSettingValue<bool>("enabled")) return;

        auto labels = LabelUtils::collectCommentLabels(m_mainLayer);
        if (labels.empty()) {
            log::warn("Unrecognised comment layout (height={}, account={})",
                m_height, m_accountComment);
            return;
        }

        ShortcutCreator::apply(
            labels, 
            menu, 
            this,
            menu_selector(IDSCommentCell::onShortcutClick),
            comment->m_levelID,
            fmt::format("comment/h{}", static_cast<int>(m_height))
        );
    }
};
