module;

#include <rhatt/base/Singleton.hpp>

#include <Geode/Geode.hpp>

export module cw.lvlcollab.util.DelegateDirector;

export namespace cw::lvlcollab {
    class DelegateDirector final : public base::Singleton<DelegateDirector>, public LevelManagerDelegate, public UserInfoDelegate, public UserListDelegate {
        using LevelCallback = geode::Function<void(geode::Result<geode::Ref<GJGameLevel>>)>;
        using UserCallback = geode::Function<void(geode::Result<geode::Ref<GJUserScore>>)>;

        using LevelListCallback = geode::Function<void(geode::Result<geode::cocos::CCArrayExt<GJGameLevel>>)>;
        using UserListCallback = geode::Function<void(geode::Result<geode::cocos::CCArrayExt<GJUserScore>>)>;

    private:
        geode::utils::StringMap<LevelCallback> m_levelCallbacks;
        geode::utils::StringMap<UserCallback> m_userCallbacks;

        geode::utils::StringMap<LevelListCallback> m_levelListCallbacks;
        std::unordered_map<UserListType, UserListCallback> m_userListCallbacks;

        std::unordered_map<int, std::vector<UserCallback>> m_userIDCallbacks;

    protected:
        void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int) override;
        void loadLevelsFailed(const char* key, int) override;

        void getUserListFinished(cocos2d::CCArray* scores, UserListType type) override;
        void getUserListFailed(UserListType type, GJErrorCode errorType) override;
        void userListChanged(cocos2d::CCArray* scores, UserListType type) override;

    public:
        void searchLevel(int levelID, LevelCallback callback);
        void searchUser(std::string username, UserCallback callback);

        void searchUserLevels(int userID, int page, LevelListCallback callback);
        void getUserList(UserListType type, UserListCallback callback);

        void searchUserByID(int accountID, UserCallback callback);
    };
};