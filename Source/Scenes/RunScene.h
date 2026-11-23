#pragma once
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "Actors/ScrollActor.h"
#include "Rules/BeatClock.h"
#include "Rules/LaneSelector.h"
#include "Rules/Melody.h"
#include "Rules/ScoreBoard.h"
#include "SpinoCore/Core/Scenes/Scene.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Sound/SoundManager.h"
#include "SpinoCore/Utils/Size2D.h"
#include "Visuals/ScreenFx.h"

namespace SpinoCore::Core::Scenes {
    class ScenesManager;
    struct StartContext;
    struct ControlContext;
}

namespace Nightmare {
    class ActorRegistry;
    class City;
    class EmberField;
    class HudScreen;
    class MenuScreen;
    class ObstacleActor;
    class PauseScreen;
    class RigCamera;
    class RunnerActor;
    class ScoresScreen;
    class SpeakerActor;
    class TargetActor;
    struct BurstSettings;

    class RunScene final : public SpinoCore::Core::Scenes::Scene {
    public:
        RunScene(SpinoCore::Core::Scenes::ScenesManager& scenes, SpinoCore::Utils::Size2D size, std::string playerName);
        ~RunScene() override;

        void OnStart(const SpinoCore::Core::Scenes::StartContext&) override {}
        void OnSetup(const SpinoCore::Core::Scenes::ControlContext& ctx) override;
        void OnUpdate(const SpinoCore::Core::Scenes::ControlContext& ctx) override;
        void OnFinish(const SpinoCore::Core::Scenes::ControlContext&) override {}

    private:
        enum class Phase {
            Menu,
            Scores,
            Intro,
            Playing,
            Paused,
            Dying,
            Finishing,
        };

        struct PendingNote final {
            float seconds{0.0f};
            int midi{60};
            float lengthBeats{1.0f};
        };

        void SetupWorld(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void SetupScreens(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void Enter(Phase phase);

        bool UpdateMenu(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void UpdateScores(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void UpdateIntro();
        bool UpdatePlaying(const SpinoCore::Core::Scenes::ControlContext& ctx);
        bool UpdatePaused(const SpinoCore::Core::Scenes::ControlContext& ctx);
        bool UpdateEnding(const SpinoCore::Core::Scenes::ControlContext& ctx);

        void StartIntro(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void BeginRun();
        void PauseRun(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void ResumeRun(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void Die(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void Finish(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void ReturnToMenu();

        void UpdateBeat();
        void SpawnTargets();
        void SpawnObstacles();
        void HandleLanes(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void HandleShot(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void HandleObstacleHits(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void HandlePassedTargets(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void PlayDueNotes(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void PlayBacking(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void ResumeHarmony(const SpinoCore::Core::Scenes::ControlContext& ctx);
        void RefreshHud();
        void Burst(const SpinoCore::Math::Vector3f& position, const BurstSettings& settings, bool additive);

        SpinoCore::Core::Scenes::ScenesManager& mScenes;
        SpinoCore::Utils::Size2D mSize;
        std::string mPlayerName;

        std::unique_ptr<ActorRegistry> mRegistry;
        std::unique_ptr<City> mCity;
        WorldMotion mMotion{};
        RigCamera* mCamera{nullptr};
        RunnerActor* mRunner{nullptr};
        EmberField* mEmbers{nullptr};
        std::array<SpeakerActor*, 2> mSpeakers{};
        std::vector<TargetActor*> mTargets;
        std::vector<ObstacleActor*> mObstacleHits;

        MenuScreen* mMenu{nullptr};
        ScoresScreen* mScores{nullptr};
        HudScreen* mHud{nullptr};
        PauseScreen* mPause{nullptr};

        ScreenFx mFx{};
        BeatClock mClock;
        MelodySchedule mMelody;
        ScoreBoard mBoard{};
        LaneSelector mLanes;
        std::vector<PendingNote> mNotes;
        std::size_t mNextBacking{0};
        std::size_t mLastPad{0};
        bool mPadStarted{false};

        SpinoCore::Sound::SoundHandle mMusic{};
        float mNextObstacleBeat{0.0f};
        int mLastBeat{-1};
        Phase mPhase{Phase::Menu};
        float mPhaseTime{0.0f};
        float mSceneTime{0.0f};
        bool mWasTyping{false};
    };
}
