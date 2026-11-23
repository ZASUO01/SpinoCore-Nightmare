#include "RigCamera.h"
#include <algorithm>
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Utils/Random.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Math::Vector3f;
        namespace Operations = SpinoCore::Math::Operations;

        constexpr float FieldOfViewDegrees = 55.0f;

        [[nodiscard]] float Lerp(const float a, const float b, const float t) {
            return a + (b - a) * t;
        }

        [[nodiscard]] CameraShot Mix(const CameraShot& a, const CameraShot& b, const float t) {
            return CameraShot{
                .yaw = Lerp(a.yaw, b.yaw, t),
                .distance = Lerp(a.distance, b.distance, t),
                .height = Lerp(a.height, b.height, t),
                .lookHeight = Lerp(a.lookHeight, b.lookHeight, t),
                .lookAhead = Lerp(a.lookAhead, b.lookAhead, t),
                .side = Lerp(a.side, b.side, t),
            };
        }

        [[nodiscard]] float SmoothStep(const float t) {
            const float x = std::clamp(t, 0.0f, 1.0f);
            return x * x * (3.0f - 2.0f * x);
        }
    }

    RigCamera::RigCamera() {
        mFov = FieldOfViewDegrees * SpinoCore::Math::Constants::DegToRad<float>;
        mNear = 0.1f;
        mFar = 260.0f;
        Cut(Shots::Menu);
    }

    void RigCamera::Cut(const CameraShot& shot) {
        mFrom = shot;
        mTo = shot;
        mCurrent = shot;
        mBlendTime = 0.0f;
        mBlendDuration = 0.0f;
        Apply();
    }

    void RigCamera::Blend(const CameraShot& shot, const float seconds) {
        mFrom = mCurrent;
        mTo = shot;
        mBlendTime = 0.0f;
        mBlendDuration = std::max(seconds, 0.001f);
    }

    void RigCamera::Shake(const float strength) {
        mShake = std::max(mShake, strength);
    }

    void RigCamera::Update(const float deltaTime) {
        mTime += deltaTime;
        if (IsBlending()) {
            mBlendTime = std::min(mBlendTime + deltaTime, mBlendDuration);
            mCurrent = Mix(mFrom, mTo, SmoothStep(mBlendTime / mBlendDuration));
        } else {
            mCurrent = mTo;
        }
        mShake = std::max(0.0f, mShake - deltaTime * 2.5f);
        mBob += (0.0f - mBob) * std::min(1.0f, deltaTime * 10.0f);
        Apply();
    }

    void RigCamera::Apply() {
        const Vector3f away{Operations::Sin(mCurrent.yaw), 0.0f, Operations::Cos(mCurrent.yaw)};
        const Vector3f across{away.z, 0.0f, -away.x};
        const Vector3f shake = mShake > 0.0f
            ? Vector3f{SpinoCore::Utils::Random::GetFloatRange(-1.0f, 1.0f), SpinoCore::Utils::Random::GetFloatRange(-1.0f, 1.0f), 0.0f} * (mShake * 0.18f)
            : Vector3f::Zero();

        mEye = mFocus + away * mCurrent.distance + Vector3f{0.0f, mCurrent.height + mBob, 0.0f} + shake;
        mTarget = mFocus + Vector3f{0.0f, mCurrent.lookHeight + mBob * 0.5f, 0.0f} - away * mCurrent.lookAhead + across * mCurrent.side;
        Apply3DView();
    }

    Vector3f RigCamera::GetForward() const {
        return (mTarget - mEye).Normalized();
    }

    float RigCamera::GetHalfHeightAt(const float depth) const {
        return depth * Operations::Tan(mFov * 0.5f);
    }

    Vector3f RigCamera::ScreenAnchor(const float ndcX, const float ndcY, const float depth) const {
        const Vector3f forward = GetForward();
        const Vector3f right = forward.Cross(Vector3f::UnitY()).Normalized();
        const Vector3f up = right.Cross(forward).Normalized();
        const float halfHeight = GetHalfHeightAt(depth);
        const float aspect = mHeight > 0.0f ? mWidth / mHeight : 16.0f / 9.0f;
        return mEye + forward * depth + right * (ndcX * halfHeight * aspect) + up * (ndcY * halfHeight);
    }
}
