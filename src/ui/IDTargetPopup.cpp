#include "IDTargetPopup.hpp"

#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>

using namespace geode::prelude;

namespace {
    constexpr float POPUP_WIDTH = 300.f;
    constexpr float POPUP_HEIGHT = 180.f;

    constexpr char const* SETTING_KEY = "id-target";
    constexpr char const* VALUE_ASK = "Ask";
    constexpr char const* VALUE_LEVEL = "Level";
    constexpr char const* VALUE_LIST = "List";
}

std::optional<IDTargetPopup::Target> IDTargetPopup::savedTarget() {
    auto value = Mod::get()->getSettingValue<std::string>(SETTING_KEY);

    if (value == VALUE_LEVEL) return Target::Level;
    if (value == VALUE_LIST) return Target::List;
    return std::nullopt;
}

IDTargetPopup* IDTargetPopup::create(int id, Callback callback) {
    auto ret = new IDTargetPopup();
    if (ret->init(id, std::move(callback))) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool IDTargetPopup::init(int id, Callback callback) {
    if (!Popup::init(POPUP_WIDTH, POPUP_HEIGHT)) return false;

    m_callback = std::move(callback);

    this->setTitle(fmt::format("Open {}", id));

    auto question = CCLabelBMFont::create("Is this a level or a list?", "bigFont.fnt");
    question->setScale(.5f);
    m_mainLayer->addChildAtPosition(question, Anchor::Center, { 0.f, 32.f });

    auto levelSpr = ButtonSprite::create("Level", "goldFont.fnt", "GJ_button_01.png", .8f);
    levelSpr->setScale(.8f);

    auto levelBtn = CCMenuItemSpriteExtra::create(
        levelSpr, this, menu_selector(IDTargetPopup::onLevel));
    levelBtn->setID("level-button"_spr);
    m_buttonMenu->addChildAtPosition(levelBtn, Anchor::Center, { -65.f, -2.f });

    auto listSpr = ButtonSprite::create("List", "goldFont.fnt", "GJ_button_01.png", .8f);
    listSpr->setScale(.8f);

    auto listBtn = CCMenuItemSpriteExtra::create(
        listSpr, this, menu_selector(IDTargetPopup::onList));
    listBtn->setID("list-button"_spr);
    m_buttonMenu->addChildAtPosition(listBtn, Anchor::Center, { 65.f, -2.f });

    m_rememberToggle = CCMenuItemToggler::createWithStandardSprites(this, nullptr, .6f);
    m_rememberToggle->setID("remember-toggle"_spr);
    m_buttonMenu->addChildAtPosition(m_rememberToggle, Anchor::Center, { -78.f, -48.f });

    auto rememberLabel = CCLabelBMFont::create("Remember my choice", "bigFont.fnt");
    rememberLabel->setScale(.4f);
    rememberLabel->setAnchorPoint({ 0.f, .5f });
    m_mainLayer->addChildAtPosition(rememberLabel, Anchor::Center, { -64.f, -48.f });

    return true;
}

void IDTargetPopup::choose(Target target) {
    if (m_rememberToggle && m_rememberToggle->isOn()) {
        Mod::get()->setSettingValue<std::string>(
            SETTING_KEY,
            target == Target::Level ? VALUE_LEVEL : VALUE_LIST
        );
    }

    auto callback = m_callback;
    this->onClose(nullptr);

    if (callback) callback(target);
}

void IDTargetPopup::onLevel(CCObject*) {
    choose(Target::Level);
}

void IDTargetPopup::onList(CCObject*) {
    choose(Target::List);
}
