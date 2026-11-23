#include "RunScene.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include "Actors/ActorRegistry.h"
#include "Actors/EffectActor.h"
#include "Actors/ModelActor.h"
#include "Actors/ObstacleActor.h"
#include "Actors/RunnerActor.h"
#include "Actors/SpeakerActor.h"
#include "Actors/TargetActor.h"
#include "Cameras/RigCamera.h"
#include "Config.h"
#include "Effects/BurstEffect.h"
#include "Effects/EmberField.h"
#include "Rules/ScoreRecords.h"
#include "Screens/HudScreen.h"
#include "Screens/MenuScreen.h"
#include "Screens/PauseScreen.h"
#include "Screens/ScoresScreen.h"
#include "Sound/Timbres.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/ActorsManager.h"
#include "SpinoCore/Core/Actors/Components/AnimatorComponent.h"
#include "SpinoCore/Core/Actors/Components/CapsuleColliderComponent.h"
#include "SpinoCore/Core/Actors/Components/ColliderComponent.h"
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"
#include "SpinoCore/Core/Cameras/CamerasManager.h"
#include "SpinoCore/Core/Input/InputState.h"
#include "SpinoCore/Core/Lighting/GlobalLight.h"
#include "SpinoCore/Core/Scenes/Contexts.h"
#include "SpinoCore/Core/Scenes/ScenesManager.h"
#include "SpinoCore/Core/UI/ScreensManager.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Math/Quaternion.h"
#include "SpinoCore/Physics/Ray.h"
#include "SpinoCore/Sound/Synth/SynthManager.h"
#include "SpinoCore/Sound/Synth/Synthesizer.h"
#include "SpinoCore/Sound/Synth/Timbre.h"
#include "SpinoCore/Utils/Random.h"
#include "Visuals/Catalog.h"
#include "Visuals/City.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace Components = SpinoCore::Core::Actors::Components;
        using SpinoCore::Core::Actors::ActorInput;
        using SpinoCore::Core::Scenes::ControlContext;
        using SpinoCore::Math::Quaternionf;
        using SpinoCore::Math::Vector2f;
        using SpinoCore::Math::Vector3f;
        using SpinoCore::Math::Vector4f;
        namespace Random = SpinoCore::Utils::Random;

        constexpr auto CameraName = "rig";
        constexpr float IntroSeconds = 2.6f;
        constexpr float DeathSeconds = 4.2f;
        constexpr float FinishSeconds = 5.5f;
        constexpr float MenuMusicVolume = 0.7f;
        constexpr float LeadVolume = 1.0f;
        constexpr float SubVolume = 0.6f;
        constexpr float PadVolume = 1.0f;
        constexpr float BassVolume = 1.0f;
        constexpr float KickVolume = 0.4f;
        constexpr float SnareVolume = 1.0f;
        constexpr float HatVolume = 0.25f;
        constexpr float ZapVolume = 0.25f;
        constexpr int ZapNote = 91;

        struct Instrument final {
            const char* name;
            BackingVoice voice;
        };

        constexpr std::array BackingInstruments{
            Instrument{Config::PadInstrument, BackingVoice::Pad},
            Instrument{Config::BassInstrument, BackingVoice::Bass},
            Instrument{Config::KickInstrument, BackingVoice::Kick},
            Instrument{Config::SnareInstrument, BackingVoice::Snare},
            Instrument{Config::HatInstrument, BackingVoice::Hat},
        };

        [[nodiscard]] const char* InstrumentFor(const BackingVoice voice) {
            for (const Instrument& instrument : BackingInstruments) {
                if (instrument.voice == voice) return instrument.name;
            }
            return Config::PadInstrument;
        }

        template <typename T>
        void RegisterInstrument(SpinoCore::Sound::Synth::SynthManager& synths, const char* name, const SpinoCore::Sound::Synth::SynthMode mode, const float volume) {
            synths.Register(name, SpinoCore::Sound::Synth::Timbre::CreateUnique<T>(), mode);
            synths.SetVolume(name, volume);
        }
        constexpr int LowestNote = 69;
        constexpr int HighestNote = 86;
        constexpr float LowestFlight = 1.6f;
        constexpr float HighestFlight = 4.7f;
        constexpr float SpeakerDepth = 2.2f;
        constexpr float SpeakerNdcX = 0.8f;
        constexpr float SpeakerNdcY = -0.93f;
        constexpr float SpeakerScale = 0.31f;
        constexpr float SpeakerHeight = 1.69f;
        constexpr float ShotRange = 120.0f;

        [[nodiscard]] Vector2f NdcToPixels(const SpinoCore::Utils::Size2D size, const float ndcX, const float ndcY) {
            return Vector2f{(ndcX + 1.0f) * 0.5f * static_cast<float>(size.width), (1.0f - ndcY) * 0.5f * static_cast<float>(size.height)};
        }

        [[nodiscard]] Quaternionf Yaw(const float degrees) {
            return Quaternionf{Vector3f::UnitY(), SpinoCore::Math::Operations::ToRadians(degrees)};
        }

        [[nodiscard]] std::filesystem::path ScoresFile() {
            char* folder = SDL_GetPrefPath("SpinoCore", "Nightmare");
            if (!folder) return std::filesystem::path("scores.txt");
            std::filesystem::path path = std::filesystem::path(folder) / "scores.txt";
            SDL_free(folder);
            return path;
        }

        [[nodiscard]] BurstSettings TargetBurst(const Vector4f& color) {
            return BurstSettings{
                .count = 70, .minSpeed = 3.0f, .maxSpeed = 11.0f, .gravity = 3.0f, .drag = 2.2f, .minLife = 0.35f, .maxLife = 0.9f,
                .startSize = 0.42f, .endSize = 0.04f, .startColor = Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
                .endColor = Vector4f{color.x, color.y, color.z, 0.0f}, .spawnRadius = 0.3f,
            };
        }

        [[nodiscard]] BurstSettings RingBurst(const Vector4f& color) {
            return BurstSettings{
                .count = 26, .minSpeed = 6.0f, .maxSpeed = 7.0f, .gravity = 0.0f, .drag = 3.5f, .minLife = 0.3f, .maxLife = 0.4f,
                .startSize = 0.9f, .endSize = 0.2f, .startColor = Vector4f{color.x, color.y, color.z, 0.7f},
                .endColor = Vector4f{color.x, color.y, color.z, 0.0f}, .spawnRadius = 0.05f,
            };
        }

        [[nodiscard]] BurstSettings MissBurst() {
            return BurstSettings{
                .count = 45, .minSpeed = 3.0f, .maxSpeed = 9.0f, .gravity = 6.0f, .drag = 1.8f, .minLife = 0.3f, .maxLife = 0.8f,
                .startSize = 0.3f, .endSize = 0.04f, .startColor = Vector4f{1.0f, 0.35f, 0.4f, 1.0f},
                .endColor = Vector4f{0.3f, 0.0f, 0.1f, 0.0f}, .spawnRadius = 0.4f,
            };
        }

        [[nodiscard]] BurstSettings FireBurst() {
            return BurstSettings{
                .count = 110, .minSpeed = 2.0f, .maxSpeed = 9.0f, .bias = Vector3f{0.0f, 3.0f, 2.0f}, .gravity = 5.0f, .drag = 1.8f,
                .minLife = 0.4f, .maxLife = 1.1f, .startSize = 0.7f, .endSize = 0.1f, .startColor = Vector4f{1.0f, 0.85f, 0.4f, 1.0f},
                .endColor = Vector4f{0.9f, 0.15f, 0.05f, 0.0f}, .spawnRadius = 0.6f,
            };
        }

        [[nodiscard]] BurstSettings SmokeBurst() {
            return BurstSettings{
                .count = 40, .minSpeed = 0.5f, .maxSpeed = 2.5f, .bias = Vector3f{0.0f, 2.2f, 3.0f}, .gravity = -0.5f, .drag = 0.8f,
                .minLife = 0.9f, .maxLife = 1.6f, .startSize = 0.8f, .endSize = 2.2f, .startColor = Vector4f{0.2f, 0.16f, 0.24f, 0.7f},
                .endColor = Vector4f{0.08f, 0.06f, 0.1f, 0.0f}, .spawnRadius = 0.6f,
            };
        }

        [[nodiscard]] BurstSettings TracerBurst(const Vector3f& to, const Vector4f& color) {
            return BurstSettings{
                .count = 48, .minSpeed = 0.0f, .maxSpeed = 0.4f, .gravity = 0.0f, .drag = 0.0f, .minLife = 0.12f, .maxLife = 0.2f,
                .startSize = 0.16f, .endSize = 0.02f, .startColor = Vector4f{color.x, color.y, color.z, 0.95f},
                .endColor = Vector4f{color.x, color.y, color.z, 0.0f}, .spawnRadius = 0.02f, .alongLine = true, .lineEnd = to,
            };
        }

        [[nodiscard]] BurstSettings SpeakerBurst(const bool downbeat) {
            const Vector4f color = downbeat ? Palette::Accent : Palette::Cyan;
            return BurstSettings{
                .count = downbeat ? 14u : 7u, .minSpeed = 0.6f, .maxSpeed = 1.6f, .bias = Vector3f{0.0f, 0.4f, 0.0f}, .gravity = 0.0f,
                .drag = 2.0f, .minLife = 0.2f, .maxLife = 0.35f, .startSize = 0.07f, .endSize = 0.01f,
                .startColor = Vector4f{color.x, color.y, color.z, 0.9f}, .endColor = Vector4f{color.x, color.y, color.z, 0.0f}, .spawnRadius = 0.08f,
            };
        }
    }

    RunScene::RunScene(SpinoCore::Core::Scenes::ScenesManager& scenes, const SpinoCore::Utils::Size2D size, std::string playerName)
        : mScenes(scenes)
        , mSize(size)
        , mPlayerName(std::move(playerName))
        , mClock(Config::Bpm, Config::FirstBeatSeconds)
        , mMelody(LevelMelody())
        , mLanes(Config::LaneCount, Config::StartLane) {}

    RunScene::~RunScene() = default;

    void RunScene::OnSetup(const ControlContext& ctx) {
        mRegistry = std::make_unique<ActorRegistry>(ctx.actorsManager);

        auto camera = std::make_unique<RigCamera>();
        mCamera = camera.get();
        ctx.camerasManager.AddCamera(std::move(camera), CameraName);
        ctx.camerasManager.SetCurrentCamera(CameraName);

        ctx.globalLight.SetPosition(Config::MoonLightPosition);
        ctx.globalLight.SetTarget(Config::MoonLightTarget);
        ctx.globalLight.SetColor(Config::MoonLightColor);
        ctx.globalLight.SetIntensity(0.75f);
        ctx.globalLight.SetAmbientColor(Config::AmbientColor);
        ctx.globalLight.SetShadowExtent(26.0f);
        ctx.globalLight.SetShadowRange(0.1f, 70.0f);

        ScreenFx::Install(ctx.postProcessManager);

        using SpinoCore::Sound::Synth::SynthMode;
        RegisterInstrument<LeadTimbre>(ctx.synthManager, Config::LeadInstrument, SynthMode::Monophonic, LeadVolume);
        RegisterInstrument<SubTimbre>(ctx.synthManager, Config::SubInstrument, SynthMode::Monophonic, SubVolume);
        RegisterInstrument<PadTimbre>(ctx.synthManager, Config::PadInstrument, SynthMode::Polyphonic, PadVolume);
        RegisterInstrument<BassTimbre>(ctx.synthManager, Config::BassInstrument, SynthMode::Monophonic, BassVolume);
        RegisterInstrument<KickTimbre>(ctx.synthManager, Config::KickInstrument, SynthMode::Monophonic, KickVolume);
        RegisterInstrument<SnareTimbre>(ctx.synthManager, Config::SnareInstrument, SynthMode::Polyphonic, SnareVolume);
        RegisterInstrument<HatTimbre>(ctx.synthManager, Config::HatInstrument, SynthMode::Polyphonic, HatVolume);
        RegisterInstrument<ZapTimbre>(ctx.synthManager, Config::ZapInstrument, SynthMode::Monophonic, ZapVolume);
        ctx.synthManager.SetMasterVolume(0.85f);
        ctx.soundManager.SetMasterVolume(0.9f);

        SetupWorld(ctx);
        SetupScreens(ctx);

        mMusic = ctx.soundManager.Play(Config::MenuTrack, true, MenuMusicVolume);
        Enter(Phase::Menu);
    }

    void RunScene::SetupWorld(const ControlContext&) {
        mCity = std::make_unique<City>(*mRegistry, mMotion);
        mCity->Build();

        auto embers = std::make_unique<EffectActor>();
        auto field = std::make_unique<EmberField>(*embers, 240);
        mEmbers = field.get();
        embers->AddComponent(std::move(field));
        mRegistry->Spawn(std::move(embers));

        auto runner = std::make_unique<RunnerActor>(Config::LaneX(mLanes.GetLane()));
        mRunner = mRegistry->Spawn(std::move(runner));
        mRunner->GetCollider().SetCollisionCallBack([this](Components::ColliderComponent& other) {
            auto* obstacle = dynamic_cast<ObstacleActor*>(other.GetActor());
            if (!obstacle || obstacle->WasHit() || mPhase != Phase::Playing) return;
            obstacle->MarkHit();
            mObstacleHits.push_back(obstacle);
        });

        auto gun = std::make_unique<ModelActor>(ActorInput{
            .position = Vector3f{-0.2f, 1.42f, 0.18f},
            .scale = Vector3f{0.055f, 0.055f, 0.055f},
        }, Config::GunModel);
        mRunner->Attach(*mRegistry->Spawn(std::move(gun), mRunner));

        for (std::size_t index = 0; index < mSpeakers.size(); ++index) {
            const float side = index == 0 ? -1.0f : 1.0f;
            mSpeakers[index] = mRegistry->Spawn(std::make_unique<SpeakerActor>(*mCamera, side * SpeakerNdcX, SpeakerNdcY, SpeakerDepth, SpeakerScale));
        }
    }

    void RunScene::SetupScreens(const ControlContext& ctx) {
        const float topNdc = SpeakerNdcY + SpeakerHeight * SpeakerScale / mCamera->GetHalfHeightAt(SpeakerDepth) + 0.16f;
        auto hud = std::make_unique<HudScreen>(mSize, NdcToPixels(mSize, -SpeakerNdcX, topNdc), NdcToPixels(mSize, SpeakerNdcX, topNdc));
        mHud = hud.get();
        ctx.screensManager.PushScreen(std::move(hud));

        auto menu = std::make_unique<MenuScreen>(mSize, ctx.soundManager, mPlayerName);
        mMenu = menu.get();
        ctx.screensManager.PushScreen(std::move(menu));
    }

    void RunScene::Enter(const Phase phase) {
        mPhase = phase;
        mPhaseTime = 0.0f;
    }

    void RunScene::OnUpdate(const ControlContext& ctx) {
        mSceneTime += Config::StepSeconds;
        mPhaseTime += Config::StepSeconds;

        bool leaving = false;
        switch (mPhase) {
            case Phase::Menu: leaving = UpdateMenu(ctx); break;
            case Phase::Scores: UpdateScores(ctx); break;
            case Phase::Intro: UpdateIntro(); break;
            case Phase::Playing: leaving = UpdatePlaying(ctx); break;
            case Phase::Paused: leaving = UpdatePaused(ctx); break;
            case Phase::Dying:
            case Phase::Finishing: leaving = UpdateEnding(ctx); break;
        }
        if (leaving) return;

        if (!mRegistry->IsPaused()) {
            mCity->Recycle();
            mRegistry->DestroyExpired();
        }
        mEmbers->SetFlowSpeed(mMotion.speed);
        mHud->Update(Config::StepSeconds);
        mFx.Update(Config::StepSeconds, ctx.postProcessManager);
    }

    bool RunScene::UpdateMenu(const ControlContext& ctx) {
        const auto& input = ctx.simulationInput;
        mMenu->Refresh(mSceneTime);
        mCamera->SetFocus(Vector3f{mRunner->GetPosition().x, 0.0f, 0.0f});

        if (mWasTyping && input.WasMouseButtonPressed(SpinoCore::Core::Input::MouseButton::Left)) {
            if (const MenuChoice choice = mMenu->ButtonAt(input.GetMousePosition()); choice != MenuChoice::None) mMenu->Choose(choice);
        }
        if (mMenu->IsTyping() && (input.WasKeyPressed(SDL_SCANCODE_RETURN) || input.WasKeyPressed(SDL_SCANCODE_KP_ENTER))) mMenu->Choose(MenuChoice::Start);
        mWasTyping = mMenu->IsTyping();

        switch (mMenu->TakeChoice()) {
            case MenuChoice::Start:
                StartIntro(ctx);
                break;
            case MenuChoice::Scores: {
                const std::vector<ScoreRecord> records = ScoreStorage{ScoresFile()}.Load();
                auto scores = std::make_unique<ScoresScreen>(mSize, ctx.soundManager, records);
                mScores = scores.get();
                ctx.screensManager.PushScreen(std::move(scores));
                mFx.KeepSharp(mSize, ScoresScreen::PanelCenter(mSize), ScoresScreen::PanelSize);
                mFx.SetBlurTarget(1.0f);
                Enter(Phase::Scores);
                break;
            }
            case MenuChoice::Quit: {
                SDL_Event quit{};
                quit.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quit);
                break;
            }
            case MenuChoice::None:
                break;
        }
        return false;
    }

    void RunScene::UpdateScores(const ControlContext& ctx) {
        mScores->Refresh();
        if (!mScores->TakeClose() && !ctx.simulationInput.WasKeyPressed(SDL_SCANCODE_ESCAPE)) return;

        ctx.screensManager.PopScreen();
        mScores = nullptr;
        mFx.SetBlurTarget(0.0f);
        Enter(Phase::Menu);
    }

    void RunScene::StartIntro(const ControlContext& ctx) {
        mPlayerName = mMenu->GetName();
        ctx.screensManager.PopScreen();
        mMenu = nullptr;
        ctx.soundManager.Stop(mMusic);
        mCamera->Blend(Shots::Play, IntroSeconds);
        mHud->ShowBanner("PREPARE-SE", mPlayerName, Palette::Cyan, IntroSeconds + 1.2f);
        Enter(Phase::Intro);
    }

    void RunScene::UpdateIntro() {
        mCamera->SetFocus(Vector3f{mRunner->GetPosition().x * 0.5f, 0.0f, 0.0f});
        if (mPhaseTime > IntroSeconds * 0.6f) mRunner->SetRunning(true);
        mMotion.speed = Config::RunSpeed * std::clamp((mPhaseTime - IntroSeconds * 0.6f) / (IntroSeconds * 0.4f), 0.0f, 1.0f);
        if (!mCamera->IsBlending()) BeginRun();
    }

    void RunScene::BeginRun() {
        mClock.Reset();
        mNextObstacleBeat = 2.0f;
        mLastBeat = -1;
        mMotion.speed = Config::RunSpeed;
        mHud->SetHudShown(true);
        for (SpeakerActor* speaker : mSpeakers) speaker->SetShown(true);
        RefreshHud();
        Enter(Phase::Playing);
    }

    bool RunScene::UpdatePlaying(const ControlContext& ctx) {
        if (ctx.simulationInput.WasKeyPressed(SDL_SCANCODE_RETURN) || ctx.simulationInput.WasKeyPressed(SDL_SCANCODE_KP_ENTER)) {
            PauseRun(ctx);
            return false;
        }

        mClock.Advance(Config::StepSeconds);
        PlayBacking(ctx);
        UpdateBeat();
        HandleLanes(ctx);
        mCamera->SetFocus(Vector3f{mRunner->GetPosition().x * 0.45f, 0.0f, 0.0f});
        mHud->SetAim(ctx.simulationInput.GetMousePosition(), ctx.simulationInput.IsMouseInWindow());

        SpawnTargets();
        SpawnObstacles();
        HandleShot(ctx);
        HandleObstacleHits(ctx);
        HandlePassedTargets(ctx);
        PlayDueNotes(ctx);

        const float released = static_cast<float>(mMelody.GetReleased()) / static_cast<float>(std::max<std::size_t>(1, mMelody.GetCount()));
        mHud->SetProgress(released);
        RefreshHud();

        if (mBoard.IsDead()) {
            Die(ctx);
        } else if (mMelody.IsExhausted() && mTargets.empty()) {
            Finish(ctx);
        }
        return false;
    }

    void RunScene::UpdateBeat() {
        const auto beat = static_cast<int>(std::floor(mClock.GetBeat()));
        if (beat == mLastBeat || beat < 0) return;
        mLastBeat = beat;

        const bool downbeat = beat % 4 == 0;
        mFx.Pulse(downbeat ? 0.65f : 0.3f);
        mCamera->SetBob(downbeat ? -0.05f : -0.025f);
        for (SpeakerActor* speaker : mSpeakers) {
            speaker->Pulse(downbeat ? 1.0f : 0.6f);
            Burst(speaker->GetConePoint(), SpeakerBurst(downbeat), true);
        }
    }

    void RunScene::HandleLanes(const ControlContext& ctx) {
        const auto& input = ctx.simulationInput;
        const bool left = input.WasKeyPressed(SDL_SCANCODE_LEFT) || input.WasKeyPressed(SDL_SCANCODE_A);
        const bool right = input.WasKeyPressed(SDL_SCANCODE_RIGHT) || input.WasKeyPressed(SDL_SCANCODE_D);
        if (left && mLanes.MoveLeft()) mRunner->MoveToLane(Config::LaneX(mLanes.GetLane()));
        if (right && mLanes.MoveRight()) mRunner->MoveToLane(Config::LaneX(mLanes.GetLane()));
    }

    void RunScene::SpawnTargets() {
        const float melodyBeat = mClock.GetBeat() - Config::LeadInBeats;
        const float speed = (Config::TargetPassZ - Config::TargetSpawnZ) / (Config::TargetTravelBeats * mClock.GetSecondsPerBeat());
        for (const MelodyNote& note : mMelody.TakeDue(melodyBeat)) {
            const float pitch = std::clamp(static_cast<float>(note.midi - LowestNote) / static_cast<float>(HighestNote - LowestNote), 0.0f, 1.0f);
            const Vector3f start{Random::GetFloatRange(-4.2f, 4.2f), LowestFlight + (HighestFlight - LowestFlight) * pitch, Config::TargetSpawnZ};
            auto target = std::make_unique<TargetActor>(start, speed, note.midi, note.length);
            TargetActor* added = mRegistry->Spawn(std::move(target));

            auto body = std::make_unique<ModelActor>(ActorInput{
                .position = Vector3f{0.0f, -0.6f, 0.0f},
                .scale = Vector3f{0.33f, 0.33f, 0.33f},
            }, note.midi % 2 == 0 ? "ghost1" : "ghost2");
            auto animator = std::make_unique<Components::AnimatorComponent>(*body);
            animator->Play(Config::GhostFly);
            body->AddComponent(std::move(animator));
            mRegistry->Spawn(std::move(body), added);

            Burst(start, RingBurst(added->GetColor()), true);
            mTargets.push_back(added);
        }
    }

    void RunScene::SpawnObstacles() {
        const float beat = mClock.GetBeat();
        const float lastSpawnBeat = Config::LeadInBeats + mMelody.GetLastBeat() - 4.0f;
        if (beat < mNextObstacleBeat || beat > lastSpawnBeat) return;
        mNextObstacleBeat = beat + Random::GetFloatRange(Config::ObstacleMinGapBeats, Config::ObstacleMaxGapBeats);

        std::array<int, Config::LaneCount> lanes{0, 1, 2, 3};
        for (std::size_t index = lanes.size() - 1; index > 0; --index) {
            std::swap(lanes[index], lanes[static_cast<std::size_t>(Random::GetIntRange(0, static_cast<int>(index)))]);
        }
        const int count = Random::GetFloatRange(0.0f, 1.0f) < 0.3f ? 2 : 1;

        for (int index = 0; index < count; ++index) {
            const Catalog::ObstacleSpec& spec = Catalog::Obstacles[static_cast<std::size_t>(Random::GetIntRange(0, static_cast<int>(Catalog::Obstacles.size()) - 1))];
            const Vector3f position{Config::LaneX(lanes[static_cast<std::size_t>(index)]), 0.0f, Config::ObstacleSpawnZ - Random::GetFloatRange(0.0f, 4.0f)};
            ObstacleActor* obstacle = mRegistry->Spawn(std::make_unique<ObstacleActor>(position, spec.halfSize, mMotion));

            const bool vehicle = spec.halfSize.z > 1.0f;
            const float yaw = vehicle ? (Random::GetIntRange(0, 1) == 0 ? 0.0f : 180.0f) : Random::GetFloatRange(-12.0f, 12.0f);
            auto model = std::make_unique<ModelActor>(ActorInput{
                .position = Yaw(yaw) * spec.offset,
                .scale = Vector3f{spec.scale, spec.scale, spec.scale},
                .rotation = Yaw(yaw),
            }, spec.model);
            mRegistry->Spawn(std::move(model), obstacle);
        }
    }

    void RunScene::HandleShot(const ControlContext& ctx) {
        const auto& input = ctx.simulationInput;
        if (!input.WasMouseButtonPressed(SpinoCore::Core::Input::MouseButton::Left)) return;

        ctx.synthManager.NoteOn(Config::ZapInstrument, ZapNote, 1.0f, 0.08f);
        mHud->Kick();
        const SpinoCore::Physics::Ray ray = mCamera->ScreenPointToRay(input.GetMousePosition());
        const auto hit = ctx.actorsManager.Raycast(ray, ShotRange, [](const Components::ColliderComponent& collider) {
            return dynamic_cast<const TargetActor*>(collider.GetActor()) != nullptr;
        });

        const Vector3f muzzle = mRunner->GetMuzzle();
        if (!hit) {
            Burst(muzzle, TracerBurst(ray.PointAt(60.0f), Palette::Muted), true);
            return;
        }

        auto* target = static_cast<TargetActor*>(hit->collider->GetActor());
        const Vector4f color = target->GetColor();
        Burst(muzzle, TracerBurst(hit->point, color), true);
        Burst(target->GetPosition(), TargetBurst(color), true);
        Burst(target->GetPosition(), RingBurst(color), true);
        mNotes.push_back(PendingNote{.seconds = mClock.NextSubdivisionSeconds(4), .midi = target->GetMidi(), .lengthBeats = target->GetLengthBeats()});
        const int previousMultiplier = mBoard.GetMultiplier();
        mBoard.RegisterHit();
        mFx.Pulse(0.45f);
        if (mBoard.GetMultiplier() > previousMultiplier) {
            mHud->ShowBanner("MULTIPLICADOR x" + std::to_string(mBoard.GetMultiplier()), "combo " + std::to_string(mBoard.GetCombo()), Palette::Gold, 1.4f);
        }

        std::erase(mTargets, target);
        mRegistry->Destroy(target);
    }

    void RunScene::HandleObstacleHits(const ControlContext& ctx) {
        for (ObstacleActor* obstacle : mObstacleHits) {
            const Vector3f position = obstacle->GetPosition() + Vector3f{0.0f, 0.6f, 0.0f};
            Burst(position, FireBurst(), true);
            Burst(position, SmokeBurst(), false);
            ctx.soundManager.Play(Config::ExplosionSound, false, 0.45f);
            mBoard.RegisterObstacle();
            mRunner->Stumble();
            mFx.Damage(1.0f);
            mCamera->Shake(1.0f);
        }
        mObstacleHits.clear();
    }

    void RunScene::HandlePassedTargets(const ControlContext& ctx) {
        std::vector<TargetActor*> passed;
        for (TargetActor* target : mTargets) {
            if (target->HasPassed()) passed.push_back(target);
        }
        for (TargetActor* target : passed) {
            Burst(target->GetPosition(), MissBurst(), true);
            ctx.soundManager.Play(Config::MissSound, false, 0.5f);
            mBoard.RegisterMiss();
            mFx.Damage(0.6f);
            mCamera->Shake(0.45f);
            std::erase(mTargets, target);
            mRegistry->Destroy(target);
        }
    }

    void RunScene::PlayDueNotes(const ControlContext& ctx) {
        const float now = mClock.GetSeconds();
        for (const PendingNote& note : mNotes) {
            if (note.seconds > now) continue;
            const float duration = mClock.GetSecondsPerBeat() * note.lengthBeats * 0.9f;
            ctx.synthManager.NoteOn(Config::LeadInstrument, note.midi, 1.0f, duration);
            ctx.synthManager.NoteOn(Config::SubInstrument, note.midi - 12, 0.8f, duration);
        }
        std::erase_if(mNotes, [now](const PendingNote& note) { return note.seconds <= now; });
    }

    void RunScene::PlayBacking(const ControlContext& ctx) {
        const std::span<const BackingEvent> backing = LevelBacking();
        const float songBeat = mClock.GetBeat() - Config::LeadInBeats;
        while (mNextBacking < backing.size() && backing[mNextBacking].beat <= songBeat) {
            const BackingEvent& event = backing[mNextBacking];
            const float duration = event.length * mClock.GetSecondsPerBeat();
            if (event.noteCount == 1) {
                ctx.synthManager.NoteOn(InstrumentFor(event.voice), event.notes[0], event.velocity, duration);
            } else {
                ctx.synthManager.PlayChord(InstrumentFor(event.voice), event.Notes(), event.velocity, duration);
            }
            if (event.voice == BackingVoice::Pad) {
                mLastPad = mNextBacking;
                mPadStarted = true;
            }
            ++mNextBacking;
        }
    }

    void RunScene::ResumeHarmony(const ControlContext& ctx) {
        if (!mPadStarted) return;
        const BackingEvent& chord = LevelBacking()[mLastPad];
        const float remaining = chord.beat + chord.length - (mClock.GetBeat() - Config::LeadInBeats);
        if (remaining > 0.25f) ctx.synthManager.PlayChord(Config::PadInstrument, chord.Notes(), chord.velocity, remaining * mClock.GetSecondsPerBeat());
    }

    void RunScene::RefreshHud() {
        mHud->SetLife(mBoard.GetLife());
        mHud->SetScore(mBoard.GetScore());
        mHud->SetCombo(mBoard.GetCombo(), mBoard.GetMultiplier());
    }

    void RunScene::PauseRun(const ControlContext& ctx) {
        mRegistry->SetPaused(true);
        ctx.synthManager.StopAll();
        auto pause = std::make_unique<PauseScreen>(mSize, ctx.soundManager);
        mPause = pause.get();
        ctx.screensManager.PushScreen(std::move(pause));
        mFx.KeepSharp(mSize, PauseScreen::PanelCenter(mSize), PauseScreen::PanelSize);
        mFx.SetBlurTarget(1.0f);
        Enter(Phase::Paused);
    }

    void RunScene::ResumeRun(const ControlContext& ctx) {
        ctx.screensManager.PopScreen();
        mPause = nullptr;
        mRegistry->SetPaused(false);
        ResumeHarmony(ctx);
        mFx.SetBlurTarget(0.0f);
        Enter(Phase::Playing);
    }

    bool RunScene::UpdatePaused(const ControlContext& ctx) {
        mPause->Refresh();
        PauseChoice choice = mPause->TakeChoice();
        if (mPhaseTime > 0.1f && (ctx.simulationInput.WasKeyPressed(SDL_SCANCODE_RETURN) || ctx.simulationInput.WasKeyPressed(SDL_SCANCODE_KP_ENTER))) {
            choice = PauseChoice::Resume;
        }

        if (choice == PauseChoice::Resume) ResumeRun(ctx);
        if (choice == PauseChoice::Quit) {
            ReturnToMenu();
            return true;
        }
        return false;
    }

    void RunScene::Die(const ControlContext& ctx) {
        ctx.soundManager.Play(Config::ExplosionSound, false, 0.55f);
        ctx.synthManager.StopAll();
        mRunner->Die();
        for (TargetActor* target : mTargets) {
            Burst(target->GetPosition(), MissBurst(), true);
            mRegistry->Destroy(target);
        }
        mTargets.clear();
        mNotes.clear();
        mCamera->Blend(Shots::Death, 1.8f);
        mFx.SetDesaturateTarget(0.85f);
        mFx.Damage(1.0f);
        mHud->SetHudShown(false);
        for (SpeakerActor* speaker : mSpeakers) speaker->SetShown(false);
        mHud->ShowBanner("VOCÊ CAIU", "o pesadelo venceu desta vez  ·  score " + std::to_string(mBoard.GetScore()), Palette::Danger, DeathSeconds);
        Enter(Phase::Dying);
    }

    void RunScene::Finish(const ControlContext&) {
        (void)ScoreStorage{ScoresFile()}.Add(ScoreRecord{.name = mPlayerName, .score = mBoard.GetScore()});
        mRunner->Celebrate();
        mCamera->Blend(Shots::Finish, 2.8f);
        mHud->SetHudShown(false);
        for (SpeakerActor* speaker : mSpeakers) speaker->SetShown(false);
        mHud->ShowBanner("FASE CONCLUÍDA",
                         "score " + std::to_string(mBoard.GetScore()) + "  ·  melhor combo " + std::to_string(mBoard.GetBestCombo()) +
                             "  ·  " + std::to_string(mBoard.GetHits()) + "/" + std::to_string(mMelody.GetCount()) + " notas",
                         Palette::Gold, FinishSeconds);
        Enter(Phase::Finishing);
    }

    bool RunScene::UpdateEnding(const ControlContext& ctx) {
        const bool dying = mPhase == Phase::Dying;
        mMotion.speed = std::max(0.0f, mMotion.speed - (dying ? 30.0f : 9.0f) * Config::StepSeconds);
        mClock.Advance(Config::StepSeconds);
        PlayDueNotes(ctx);

        if (!dying) PlayBacking(ctx);

        const float duration = dying ? DeathSeconds : FinishSeconds;
        if (mPhaseTime > duration - 0.8f) mFx.SetFadeTarget(1.0f);
        if (mPhaseTime < duration) return false;

        ReturnToMenu();
        return true;
    }

    void RunScene::Burst(const Vector3f& position, const BurstSettings& settings, const bool additive) {
        auto actor = std::make_unique<EffectActor>(ActorInput{.position = position});
        Components::ParticleEffectsInput input{};
        input.capacity = settings.count;
        input.blend = additive ? Components::ParticleBlend::Additive : Components::ParticleBlend::Alpha;
        actor->AddComponent(std::make_unique<BurstEffect>(*actor, input, settings));
        mRegistry->Spawn(std::move(actor));
    }

    void RunScene::ReturnToMenu() {
        SpinoCore::Core::Scenes::ScenesManager& scenes = mScenes;
        const SpinoCore::Utils::Size2D size = mSize;
        std::string name = mPlayerName;
        scenes.PopScene();
        scenes.PushScene(std::make_unique<RunScene>(scenes, size, std::move(name)));
    }
}
