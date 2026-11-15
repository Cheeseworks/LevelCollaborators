#pragma once

#include <rhatt/base/Singleton.hpp>

namespace cw::levelcollab {
    class DelegateDirector final : public base::Singleton<DelegateDirector>, public LevelManagerDelegate, public UserInfoDelegate {
        using LevelSearchCallback = geode::Function<void(geode::Result<GJGameLevel*>)>;
        using UserSearchCallback = geode::Function<void(geode::Result<GJUserScore*>)>;

    private:
        geode::utils::StringMap<LevelSearchCallback> m_levelCallbacks;
        geode::utils::StringMap<UserSearchCallback> m_userCallbacks;

        std::unordered_map<int, std::vector<UserSearchCallback>> m_userIDCallbacks;

    protected:
        void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type) override;
        void loadLevelsFailed(const char* key, int type) override;

        void getUserInfoFinished(GJUserScore* score) override;
        void getUserInfoFailed(int id) override;

    public:
        void searchLevel(int levelID, LevelSearchCallback callback);
        void searchUser(std::string username, UserSearchCallback callback);

        void searchUserByID(int accountID, UserSearchCallback callback);
    };
};