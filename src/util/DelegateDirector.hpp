#pragma once

#include <rhatt/base/Singleton.hpp>

#include <Geode/Geode.hpp>

namespace cw::lvlcollab {
    class DelegateDirector final : public base::Singleton<DelegateDirector>, public LevelManagerDelegate, public UserInfoDelegate {
        using LevelCallback = geode::Function<void(geode::Result<geode::Ref<GJGameLevel>>)>;
        using UserCallback = geode::Function<void(geode::Result<geode::Ref<GJUserScore>>)>;

        using LevelListCallback = geode::Function<void(geode::Result<geode::cocos::CCArrayExt<GJGameLevel*>>)>;

    private:
        geode::utils::StringMap<LevelCallback> m_levelCallbacks;
        geode::utils::StringMap<UserCallback> m_userCallbacks;

        geode::utils::StringMap<LevelListCallback> m_levelListCallbacks;

        std::unordered_map<int, std::vector<UserCallback>> m_userIDCallbacks;

    protected:
        void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int) override;
        void loadLevelsFailed(const char* key, int) override;

        void getUserInfoFinished(GJUserScore* score) override;
        void getUserInfoFailed(int id) override;

    public:
        void searchLevel(int levelID, LevelCallback callback);
        void searchUser(std::string username, UserCallback callback);

        void searchUserLevels(int userID, int page, LevelListCallback callback);

        void searchUserByID(int accountID, UserCallback callback);
    };
};