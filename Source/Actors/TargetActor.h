#pragma once
#include "GameActor.h"
#include "SpinoCore/Math/Vector.h"

namespace Nightmare {
    class TargetActor final : public GameActor {
    public:
        TargetActor(const SpinoCore::Math::Vector3f& start, float speed, int midi, float lengthBeats);

        [[nodiscard]] int GetMidi() const { return mMidi; }
        [[nodiscard]] float GetLengthBeats() const { return mLengthBeats; }
        [[nodiscard]] const SpinoCore::Math::Vector4f& GetColor() const { return mColor; }
        [[nodiscard]] bool HasPassed() const;

    protected:
        void OnUpdate(float deltaTime) override;

    private:
        SpinoCore::Math::Vector3f mBase;
        SpinoCore::Math::Vector4f mColor;
        float mSpeed;
        float mTime{0.0f};
        float mPhase;
        int mMidi;
        float mLengthBeats;
    };
}
