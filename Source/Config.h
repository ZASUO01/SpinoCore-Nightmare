#pragma once
#include "SpinoCore/Math/Vector.h"

namespace Nightmare::Config {
    inline constexpr float StepSeconds = 1.0f / 60.0f;

    inline constexpr int LaneCount = 4;
    inline constexpr int StartLane = 1;
    inline constexpr float LaneWidth = 2.0f;
    [[nodiscard]] constexpr float LaneX(const int lane) { return (static_cast<float>(lane) - 1.5f) * LaneWidth; }
    inline constexpr float LaneChangeSeconds = 0.14f;

    inline constexpr float RunSpeed = 10.0f;
    inline constexpr float RecycleZ = 14.0f;
    inline constexpr float WorldDepth = 140.0f;

    inline constexpr float Bpm = 130.0f;
    inline constexpr float FirstBeatSeconds = 0.05f;
    inline constexpr float LeadInBeats = 8.0f;
    inline constexpr float TargetTravelBeats = 5.0f;
    inline constexpr float TargetSpawnZ = -48.0f;
    inline constexpr float TargetPassZ = 1.0f;
    inline constexpr float TargetHitRadius = 0.95f;
    inline constexpr float EndingBeats = 6.0f;

    inline constexpr float ObstacleSpawnZ = -110.0f;
    inline constexpr float ObstacleMinGapBeats = 1.5f;
    inline constexpr float ObstacleMaxGapBeats = 3.5f;

    inline constexpr auto FontId = "mono";
    inline constexpr auto AimTexture = "aim";
    inline constexpr auto CharacterModel = "character";
    inline constexpr auto GunModel = "gun";
    inline constexpr auto StreetModel = "street";
    inline constexpr auto SpeakerModel = "speaker1";
    inline constexpr auto SmallSpeakerModel = "speaker2";

    inline constexpr auto MenuTrack = "menuMusic";
    inline constexpr auto MissSound = "miss";
    inline constexpr auto ExplosionSound = "explosion";
    inline constexpr auto ClickSound = "click";
    inline constexpr auto HoverSound = "hover";
    inline constexpr auto SelectSound = "select";

    inline constexpr auto LeadInstrument = "lead";
    inline constexpr auto SubInstrument = "sub";
    inline constexpr auto PadInstrument = "pad";
    inline constexpr auto BassInstrument = "bass";
    inline constexpr auto KickInstrument = "kick";
    inline constexpr auto SnareInstrument = "snare";
    inline constexpr auto HatInstrument = "hat";
    inline constexpr auto ZapInstrument = "zap";
    inline constexpr auto PostProcessName = "Nightmare";

    inline constexpr auto ClipIdle = "CharacterArmature|Idle";
    inline constexpr auto ClipRun = "CharacterArmature|Run_Shoot";
    inline constexpr auto ClipHit = "CharacterArmature|HitRecieve";
    inline constexpr auto ClipDeath = "CharacterArmature|Death";
    inline constexpr auto ClipWave = "CharacterArmature|Wave";
    inline constexpr auto GhostFly = "CharacterArmature|Fast_Flying";

    inline constexpr SpinoCore::Math::Vector3f MoonLightPosition{-14.0f, 22.0f, 10.0f};
    inline constexpr SpinoCore::Math::Vector3f MoonLightTarget{0.0f, 0.0f, -6.0f};
    inline constexpr SpinoCore::Math::Vector3f MoonLightColor{0.62f, 0.66f, 1.0f};
    inline constexpr SpinoCore::Math::Vector3f AmbientColor{0.12f, 0.08f, 0.2f};
}
