#pragma once

#include "DLL.hpp"

#include <Geode/Result.hpp>
#include <Geode/Bindings.hpp>

#include <Geode/utils/ZStringView.hpp>

namespace cw::lvlcollab {
    // Type of work a collaborator did
    enum class CollaboratorType : uint8_t {
        SpecialThanks,  // Thank them for being there, or did miscellaneous work!
        Playtesting,    // Tested the level for bugs and gameplay quality
        Optimization,   // Optimized groups and/or objects
        Effects,        // Added additional visual effects
        Merging,        // Merged different sections of the level
        Gameplay,       // Worked on the layout and general gameplay
        Decoration,     // Decorated the level
        Art,            // Created custom art and/or themed assets for the level
        Music,          // Composed or selected the music for the level
        Verifier,       // Verified the level
    };

    struct CollaboratorIcon final {
    private:
        int m_icon;
        IconType m_type;
        int m_color1;
        int m_color2;
        int m_glow;
        bool m_useGlow;

    protected:
        CollaboratorIcon(
            int icon,
            IconType type,
            int color1,
            int color2,
            int glow,
            bool useGlow
        );

    public:
        CollaboratorIcon() = default;

        static CollaboratorIcon create(int icon, IconType type, int color1, int color2, int glow, bool useGlow);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getIcon() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL IconType getIconType() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getColor1() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getColor2() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getGlow() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL bool useGlow() const noexcept;

        CW_LEVELCOLLAB_API_DLL SimplePlayer* createIcon() const;
    };

    struct Collaborator final {
    private:
        int m_accountID;
        int m_userID;
        std::string m_name;
        CollaboratorIcon m_icon;
        CollaboratorType m_type;

    protected:
        Collaborator(
            int accountID,
            int userID,
            std::string name,
            CollaboratorIcon icon,
            CollaboratorType type
        );

    public:
        Collaborator() = default;

        static Collaborator create(int accountID, int userID, std::string name, CollaboratorIcon icon, CollaboratorType type);
        static void createAsync(int accountID, int userID, CollaboratorType type, geode::CopyableFunction<void(geode::Result<Collaborator>)> callback);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getAccountID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getUserID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL geode::ZStringView getName() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL CollaboratorIcon const& getIcon() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL CollaboratorType getType() const noexcept;

        void reloadInfo(geode::CopyableFunction<void(geode::Result<Collaborator>)>&& callback);
    };

    struct Collaboration final {
    private:
        uint64_t m_id = 0;
        int m_levelID = -1;
        std::string m_levelName;
        Collaborator m_owner;
        std::vector<Collaborator> m_collaborators;

    protected:
        Collaboration(
            uint64_t id,
            int levelID,
            std::string levelName,
            Collaborator owner,
            std::vector<Collaborator> collaborators
        );

    public:
        Collaboration() = default;

        static Collaboration create(uint64_t id, int levelID, std::string levelName, Collaborator owner, std::vector<Collaborator> collaborators);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL uint64_t getID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getLevelID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL geode::ZStringView getLevelName() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL Collaborator const& getOwner() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL std::span<const Collaborator> getCollaborators() const noexcept;

        CW_LEVELCOLLAB_API_DLL GJGameLevel* getGJLevel() const;
        CW_LEVELCOLLAB_API_DLL std::string getFormattedString(bool prefix = true) const;
    };

    class CW_LEVELCOLLAB_API_DLL CollaborationManager final {
        friend class Collaboration;
        friend class Collaborator;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        CollaborationManager();
        ~CollaborationManager();

        CollaborationManager(const CollaborationManager&) = delete;
        CollaborationManager& operator=(const CollaborationManager&) = delete;

        CollaborationManager(CollaborationManager&&) = delete;
        CollaborationManager& operator=(CollaborationManager&&) = delete;

        void registerCollab(Collaboration collab);

        geode::Result<Collaboration> getCollab(int levelID) const noexcept;

    public:
        static CollaborationManager* get() noexcept;

        std::vector<Collaboration> getCollabs() const noexcept;

        geode::Result<Collaboration> getCollabForLevel(int levelID) const noexcept;
    };

    CW_LEVELCOLLAB_API_DLL void requestCollabForLevel(int levelID, geode::CopyableFunction<void(Collaboration)>&& callback);
    CW_LEVELCOLLAB_API_DLL void getCollaboratorInfo(int accountID, geode::CopyableFunction<void(geode::Result<GJUserScore*>)>&& callback);

    CW_LEVELCOLLAB_API_DLL geode::ZStringView getCollabTypeName(CollaboratorType type) noexcept;
};