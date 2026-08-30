#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

#include <functional>

class IDTargetPopup : public geode::Popup {
public:
    enum class Target { Level, List };

    using Callback = std::function<void(Target)>;

    static IDTargetPopup* create(int id, Callback callback);

    static std::optional<Target> savedTarget();

protected:
    bool init(int id, Callback callback);

    void onLevel(cocos2d::CCObject*);
    void onList(cocos2d::CCObject*);

    void choose(Target target);

    Callback m_callback;
    CCMenuItemToggler* m_rememberToggle = nullptr;
};
