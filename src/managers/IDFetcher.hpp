#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelManagerDelegate.hpp>

#include <functional>

class IDFetcher : public LevelManagerDelegate {
public:
    using LevelCallback = std::function<void(GJGameLevel*)>;
    using ListCallback = std::function<void(GJLevelList*)>;

    static IDFetcher* get();

    bool fetchLevel(int levelID, LevelCallback callback,
        cocos2d::CCLayer* spinnerParent = nullptr);

    bool fetchList(int listID, ListCallback callback,
        cocos2d::CCLayer* spinnerParent = nullptr);

    bool isBusy() const { return m_busy; }

    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key, int type) override;
    void loadLevelsFailed(char const* key) override;
    void loadLevelsFailed(char const* key, int type) override;

private:
    enum class Kind { Level, List };

    GJGameLevel* pickLevel(cocos2d::CCArray* levels) const;
    GJLevelList* pickList(cocos2d::CCArray* lists) const;

    void beginRequest(Kind kind, int id, cocos2d::CCLayer* spinnerParent);

    void finish(cocos2d::CCArray* results);

    void fail();

    LevelCallback m_levelCallback;
    ListCallback m_listCallback;
    LevelManagerDelegate* m_previousDelegate = nullptr;
    geode::Ref<LoadingCircle> m_circle = nullptr;
    Kind m_kind = Kind::Level;
    int m_targetID = 0;
    bool m_busy = false;
};
