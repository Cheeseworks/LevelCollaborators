#pragma once

#include <Geode/Geode.hpp>

#include <Geode/ui/Style.hpp>

namespace cw::levelcollab {
    enum class CollabMenuType : uint8_t {};

    class CollabDashboardMenu : public cocos2d::CCNode {
    protected:
        virtual bool init(cocos2d::CCSize const& size);
    };

    namespace menu {
        class DashboardInvites final : public CollabDashboardMenu {
        protected:
            bool init(cocos2d::CCSize const& size) override;

        public:
            static DashboardInvites* create();
        };

        class DashboardManage final : public CollabDashboardMenu {
        protected:
            bool init(cocos2d::CCSize const& size) override;

        public:
            static DashboardManage* create();
        };
    };

    class CollabDashboard final : public cocos2d::CCLayer {
    private:
        std::unordered_map<CollabMenuType, geode::Ref<CollabDashboardMenu>> m_menus;
        std::unordered_map<CollabMenuType, geode::Ref<geode::GeodeTabSprite>> m_tabs;

    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static CollabDashboard* create();
    };
};