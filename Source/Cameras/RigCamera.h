#pragma once
#include "SpinoCore/Core/Cameras/Camera.h"
#include "SpinoCore/Math/Vector.h"

namespace Nightmare {
    struct CameraShot final {
        float yaw{0.0f};
        float distance{6.0f};
        float height{3.0f};
        float lookHeight{1.2f};
        float lookAhead{0.0f};
        float side{0.0f};
    };

    namespace Shots {
        inline constexpr CameraShot Menu{.yaw = 3.14159265f, .distance = 4.4f, .height = 1.45f, .lookHeight = 1.05f, .lookAhead = 0.0f, .side = 1.25f};
        inline constexpr CameraShot Play{.yaw = 0.0f, .distance = 6.4f, .height = 3.5f, .lookHeight = 1.3f, .lookAhead = 11.0f, .side = 0.0f};
        inline constexpr CameraShot Death{.yaw = 0.9f, .distance = 3.2f, .height = 2.4f, .lookHeight = 0.1f, .lookAhead = 0.6f, .side = 0.0f};
        inline constexpr CameraShot Finish{.yaw = 3.14159265f, .distance = 3.6f, .height = 1.55f, .lookHeight = 1.15f, .lookAhead = 0.0f, .side = 0.0f};
    }

    class RigCamera final : public SpinoCore::Core::Cameras::Camera {
    public:
        RigCamera();

        void Update(float deltaTime) override;

        void Cut(const CameraShot& shot);
        void Blend(const CameraShot& shot, float seconds);
        [[nodiscard]] bool IsBlending() const { return mBlendTime < mBlendDuration; }
        void SetFocus(const SpinoCore::Math::Vector3f& focus) { mFocus = focus; }
        void Shake(float strength);
        void SetBob(const float bob) { mBob = bob; }

        [[nodiscard]] SpinoCore::Math::Vector3f ScreenAnchor(float ndcX, float ndcY, float depth) const;
        [[nodiscard]] SpinoCore::Math::Vector3f GetForward() const;
        [[nodiscard]] float GetHalfHeightAt(float depth) const;

    private:
        void Apply();

        CameraShot mFrom{};
        CameraShot mTo{};
        CameraShot mCurrent{};
        float mBlendTime{0.0f};
        float mBlendDuration{0.0f};
        SpinoCore::Math::Vector3f mFocus{SpinoCore::Math::Vector3f::Zero()};
        float mShake{0.0f};
        float mBob{0.0f};
        float mTime{0.0f};
    };
}
