#include "IDFetcher.hpp"

#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJLevelList.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/LoadingCircle.hpp>

using namespace geode::prelude;

IDFetcher* IDFetcher::get() {
    static IDFetcher instance;
    return &instance;
}

void IDFetcher::beginRequest(Kind kind, int id, CCLayer* spinnerParent) {
    auto glm = GameLevelManager::sharedState();

    m_busy = true;
    m_kind = kind;
    m_targetID = id;

    m_previousDelegate = glm->m_levelManagerDelegate;
    glm->m_levelManagerDelegate = this;

    if (spinnerParent) {
        m_circle = LoadingCircle::create();
        if (m_circle) {
            m_circle->setParentLayer(spinnerParent);
            m_circle->show();
        }
    }
}

bool IDFetcher::fetchLevel(int levelID, LevelCallback callback, CCLayer* spinnerParent) {
    if (m_busy) return false;
    if (levelID <= 0 || !callback) return false;

    auto glm = GameLevelManager::sharedState();
    if (!glm) return false;

    auto searchObject = GJSearchObject::create(SearchType::Search, std::to_string(levelID));
    if (!searchObject) return false;

    m_targetID = levelID;
    m_kind = Kind::Level;

    if (auto saved = glm->getSavedLevel(levelID)) {
        if (!saved->m_levelName.empty()) {
            callback(saved);
            return true;
        }
    }

    if (auto stored = glm->getStoredOnlineLevels(searchObject->getKey())) {
        if (auto level = pickLevel(stored)) {
            callback(level);
            return true;
        }
    }

    m_levelCallback = std::move(callback);
    beginRequest(Kind::Level, levelID, spinnerParent);

    glm->getOnlineLevels(searchObject);
    return true;
}

bool IDFetcher::fetchList(int listID, ListCallback callback, CCLayer* spinnerParent) {
    if (m_busy) return false;
    if (listID <= 0 || !callback) return false;

    auto glm = GameLevelManager::sharedState();
    if (!glm) return false;

    auto searchObject = GJSearchObject::create(SearchType::Search, std::to_string(listID));
    if (!searchObject) return false;

    searchObject->m_searchMode = 1;

    m_targetID = listID;
    m_kind = Kind::List;

    if (auto saved = glm->getSavedLevelList(listID)) {
        if (!saved->m_listName.empty()) {
            callback(saved);
            return true;
        }
    }

    if (auto stored = glm->getStoredOnlineLevels(searchObject->getKey())) {
        if (auto list = pickList(stored)) {
            callback(list);
            return true;
        }
    }

    m_listCallback = std::move(callback);
    beginRequest(Kind::List, listID, spinnerParent);

    glm->getLevelLists(searchObject);
    return true;
}

GJGameLevel* IDFetcher::pickLevel(CCArray* levels) const {
    if (!levels || levels->count() == 0) return nullptr;

    for (unsigned int i = 0; i < levels->count(); i++) {
        auto level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(i));
        if (level && level->m_levelID.value() == m_targetID) return level;
    }

    return typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0));
}

GJLevelList* IDFetcher::pickList(CCArray* lists) const {
    if (!lists || lists->count() == 0) return nullptr;

    for (unsigned int i = 0; i < lists->count(); i++) {
        auto list = typeinfo_cast<GJLevelList*>(lists->objectAtIndex(i));
        if (list && list->m_listID == m_targetID) return list;
    }

    return typeinfo_cast<GJLevelList*>(lists->objectAtIndex(0));
}

void IDFetcher::finish(CCArray* results) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelManagerDelegate == this) {
            glm->m_levelManagerDelegate = m_previousDelegate;
        }
    }
    m_previousDelegate = nullptr;

    if (m_circle) {
        m_circle->fadeAndRemove();
        m_circle = nullptr;
    }

    m_busy = false;

    auto levelCallback = std::move(m_levelCallback);
    auto listCallback = std::move(m_listCallback);
    m_levelCallback = nullptr;
    m_listCallback = nullptr;

    if (m_kind == Kind::Level) {
        if (levelCallback) levelCallback(pickLevel(results));
    }
    else {
        if (listCallback) listCallback(pickList(results));
    }
}

void IDFetcher::fail() {
    finish(nullptr);
}

void IDFetcher::loadLevelsFinished(CCArray* levels, char const* key) {
    if (!m_busy) return;
    finish(levels);
}

void IDFetcher::loadLevelsFinished(CCArray* levels, char const* key, int type) {
    IDFetcher::loadLevelsFinished(levels, key);
}

void IDFetcher::loadLevelsFailed(char const* key) {
    if (!m_busy) return;
    fail();
}

void IDFetcher::loadLevelsFailed(char const* key, int type) {
    IDFetcher::loadLevelsFailed(key);
}
