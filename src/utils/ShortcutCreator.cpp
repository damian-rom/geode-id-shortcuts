#include "ShortcutCreator.hpp"

#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJLevelList.hpp>
#include <Geode/binding/LevelInfoLayer.hpp>
#include <Geode/binding/LevelListLayer.hpp>

#include "../managers/IDFetcher.hpp"
#include "../ui/IDTargetPopup.hpp"
#include "../ui/ShortcutOverlay.hpp"
#include "TextParser.hpp"
#include "LabelUtils.hpp"

using namespace geode::prelude;

int ShortcutCreator::apply(
    std::vector<CCLabelBMFont*> const& labels,
    CCNode* menu,
    CCObject* target,
    SEL_MenuHandler callback,
    int skipLevelID,
    std::string_view context
) {
    auto minDigits = static_cast<int>(Mod::get()->getSettingValue<int64_t>("min-digits"));
    auto color = Mod::get()->getSettingValue<ccColor3B>("shortcut-color");

    bool debug = Mod::get()->getSettingValue<bool>("debug-mode");

    if (debug) {
        log::debug("[{}] {} label(s), menu={}, minDigits={}, skipID={}",
            context, labels.size(), menu ? "ok" : "NULL", minDigits, skipLevelID
        );
    }

    if (labels.empty() || !menu) return 0;

    int created = 0;

    for (size_t i = 0; i < labels.size(); i++) {
        auto label = labels[i];
        if (!label) continue;

        std::string text = label->getString();

        std::string following;
        if (i + 1 < labels.size() && labels[i + 1]) following = labels[i + 1]->getString();

        auto shortcuts = TextParser::findShortcuts(text, minDigits, following);

        if (debug) {
            log::debug("[{}]   line {}: {} shortcut(s) in '{}'",
                context, i, shortcuts.size(), text);
        }

        for (auto const& shortcut : shortcuts) {
            if (skipLevelID != 0 && shortcut.id == skipLevelID) {
                if (debug) log::debug("[{}]     skipped {} (current level)", context, shortcut.id);
                continue;
            }

            LabelUtils::colorRange(label, shortcut.start, shortcut.end, color);

            auto overlay = ShortcutOverlay::add(
                menu, label,
                shortcut.start, shortcut.end, shortcut.id,
                created,
                target, callback
            );

            if (debug) {
                log::debug("[{}]     shortcut {} at [{}, {}) overlay={}",
                    context, shortcut.id, shortcut.start, shortcut.end,
                    overlay ? "ok" : "FAILED");
            }

            if (overlay) created++;
        }
    }

    return created;
}

void ShortcutCreator::openLevel(int id, CCLayer* spinnerParent) {
    auto originScene = CCDirector::sharedDirector()->getRunningScene();

    bool started = IDFetcher::get()->fetchLevel(
        id,
        [id, originScene](GJGameLevel* level) {
            if (CCDirector::sharedDirector()->getRunningScene() != originScene) return;

            if (!level) {
                FLAlertLayer::create(
                    "Level Not Found",
                    fmt::format("Could not find a level with ID <cy>{}</c>.", id),
                    "OK"
                )->show();
                return;
            }

            CCDirector::sharedDirector()->pushScene(
                CCTransitionFade::create(0.5f, LevelInfoLayer::scene(level, false))
            );
        },
        spinnerParent
    );

    if (!started) log::debug("ignored click on {} - a lookup is already running", id);
}

void ShortcutCreator::openList(int id, CCLayer* spinnerParent) {
    auto originScene = CCDirector::sharedDirector()->getRunningScene();

    bool started = IDFetcher::get()->fetchList(
        id,
        [id, originScene](GJLevelList* list) {
            if (CCDirector::sharedDirector()->getRunningScene() != originScene) return;

            if (!list) {
                FLAlertLayer::create(
                    "List Not Found",
                    fmt::format("Could not find a list with ID <cy>{}</c>.", id),
                    "OK"
                )->show();
                return;
            }

            CCDirector::sharedDirector()->pushScene(
                CCTransitionFade::create(0.5f, LevelListLayer::scene(list))
            );
        },
        spinnerParent
    );

    if (!started) log::debug("ignored click on {} - a lookup is already running", id);
}

void ShortcutCreator::handleClick(CCObject* sender, CCLayer* spinnerParent) {
    if (Mod::get()->getSettingValue<bool>("require-ctrl") &&
        !ShortcutOverlay::openModifierHeld()) {
        return;
    }

    int id = ShortcutOverlay::idOf(sender);
    if (id <= 0) return;

    if (auto target = IDTargetPopup::savedTarget()) {
        if (*target == IDTargetPopup::Target::Level) openLevel(id, spinnerParent);
        else openList(id, spinnerParent);
        return;
    }

    auto popup = IDTargetPopup::create(id, [id](IDTargetPopup::Target target) {
        if (target == IDTargetPopup::Target::Level) openLevel(id, nullptr);
        else openList(id, nullptr);
    });

    if (popup) popup->show();
}
