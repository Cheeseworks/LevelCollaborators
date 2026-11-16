#pragma once

#include <Geode/Geode.hpp>

#include <Geode/ui/Style.hpp>

namespace cw::lvlcollab {
    enum class CollabMenuType : uint8_t {
        Invites,
        Manage,
        Credits,
    };

    class CollabDashboardMenu : public cocos2d::CCNode {
    protected:
        virtual bool init(cocos2d::CCSize const& size);
    };

    namespace menu {
        class DashboardInvites final : public CollabDashboardMenu {
        protected:
            bool init(cocos2d::CCSize const& size) override;

        public:
            static DashboardInvites* create(cocos2d::CCSize const& size);
        };

        class DashboardManage final : public CollabDashboardMenu {
        protected:
            bool init(cocos2d::CCSize const& size) override;

        public:
            static DashboardManage* create(cocos2d::CCSize const& size);
        };

        class DashboardCredits final : public CollabDashboardMenu {
        protected:
            bool init(cocos2d::CCSize const& size) override;

        public:
            static DashboardCredits* create(cocos2d::CCSize const& size);
        };
    };

    class CollabDashboard final : public cocos2d::CCLayer {
    private:
        std::unordered_map<CollabMenuType, geode::Ref<CollabDashboardMenu>> m_menus;
        std::unordered_map<CollabMenuType, geode::Ref<geode::GeodeTabSprite>> m_tabs;

        void switchMenu(CollabMenuType type);

    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static CollabDashboard* create();
    };
};