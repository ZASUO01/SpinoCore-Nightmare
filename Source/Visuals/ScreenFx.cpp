#include "ScreenFx.h"
#include <algorithm>
#include <cstddef>
#include <utility>
#include "Config.h"
#include "SpinoCore/Graphics/PostProcess.h"
#include "SpinoCore/Graphics/PostProcessManager.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Graphics::PostProcessUniform;
        using SpinoCore::Graphics::UniformType;

        constexpr std::size_t PulseSlot = 0;
        constexpr std::size_t DamageSlot = 1;
        constexpr std::size_t BlurSlot = 2;
        constexpr std::size_t DesaturateSlot = 3;
        constexpr std::size_t FadeSlot = 4;
        constexpr std::size_t SharpSlot = 5;
        constexpr float SharpMargin = 14.0f;

        constexpr auto Shader = R"(
            layout (location = 0) uniform float uPulse;
            layout (location = 1) uniform float uDamage;
            layout (location = 2) uniform float uBlur;
            layout (location = 3) uniform float uDesaturate;
            layout (location = 4) uniform float uFade;
            layout (location = 5) uniform vec4 uSharpRect;

            const vec3 SkyKey = vec3(0.54, 0.64, 0.72);

            float Hash(vec2 p) {
                p = fract(p * vec2(123.34, 456.21));
                p += dot(p, p + 45.32);
                return fract(p.x * p.y);
            }

            float Noise(vec2 p) {
                vec2 i = floor(p);
                vec2 f = fract(p);
                vec2 u = f * f * (3.0 - 2.0 * f);
                return mix(mix(Hash(i), Hash(i + vec2(1.0, 0.0)), u.x), mix(Hash(i + vec2(0.0, 1.0)), Hash(i + vec2(1.0, 1.0)), u.x), u.y);
            }

            vec3 Sky(vec2 uv) {
                float aspect = uResolution.x / max(uResolution.y, 1.0);
                vec3 horizon = vec3(0.34, 0.07, 0.36);
                vec3 middle = vec3(0.09, 0.03, 0.17);
                vec3 top = vec3(0.01, 0.01, 0.04);
                float h = clamp((uv.y - 0.3) / 0.7, 0.0, 1.0);
                vec3 color = mix(horizon, middle, smoothstep(0.0, 0.35, h));
                color = mix(color, top, smoothstep(0.3, 1.0, h));

                vec2 p = vec2(uv.x * aspect, uv.y);
                float clouds = Noise(p * 3.0 + vec2(uTime * 0.03, 0.0)) * 0.6 + Noise(p * 7.0 - vec2(uTime * 0.05, 0.0)) * 0.4;
                color += vec3(0.22, 0.05, 0.28) * smoothstep(0.55, 0.95, clouds) * (1.0 - h) * 0.6;

                vec2 cell = floor(p * 90.0);
                float star = Hash(cell);
                if (star > 0.985) {
                    vec2 local = fract(p * 90.0) - 0.5;
                    float twinkle = 0.6 + 0.4 * sin(uTime * (2.0 + star * 6.0) + star * 40.0);
                    color += vec3(0.85, 0.85, 1.0) * smoothstep(0.18, 0.0, length(local)) * twinkle * smoothstep(0.1, 0.5, h);
                }

                vec2 moon = vec2(0.78 * aspect, 0.8);
                float d = length(p - moon);
                color += vec3(0.95, 0.4, 0.55) * smoothstep(0.075, 0.068, d);
                color += vec3(0.7, 0.15, 0.4) * 0.35 * exp(-d * 7.0);
                return color;
            }

            vec3 Scene(vec2 uv) {
                vec3 color = texture(uScene, uv).rgb;
                vec3 delta = abs(color - SkyKey);
                if (max(delta.r, max(delta.g, delta.b)) < 0.006) color = Sky(uv);
                return color;
            }

            vec3 Blurred(vec2 uv) {
                vec2 texel = uBlur * 3.0 / uResolution;
                vec3 sum = vec3(0.0);
                float total = 0.0;
                for (int x = -3; x <= 3; ++x) {
                    for (int y = -3; y <= 3; ++y) {
                        float w = exp(-float(x * x + y * y) / 8.0);
                        sum += Scene(uv + vec2(float(x), float(y)) * texel) * w;
                        total += w;
                    }
                }
                return sum / total;
            }

            void main() {
                vec2 uv = fragTextureCoord;
                vec2 centered = uv - 0.5;
                float shift = 0.0012 + uPulse * 0.0035 + uDamage * 0.01;

                vec3 color;
                bool sharp = uv.x >= uSharpRect.x && uv.y >= uSharpRect.y && uv.x <= uSharpRect.z && uv.y <= uSharpRect.w;
                if (uBlur > 0.01 && !sharp) {
                    color = Blurred(uv);
                } else {
                    color.r = Scene(uv + centered * shift).r;
                    color.g = Scene(uv).g;
                    color.b = Scene(uv - centered * shift).b;
                }

                float edge = length(centered * vec2(uResolution.x / max(uResolution.y, 1.0), 1.0));
                float vignette = smoothstep(0.95, 0.35, edge);
                color *= mix(0.42, 1.0, vignette);
                color += vec3(0.45, 0.08, 0.55) * uPulse * (1.0 - vignette) * 0.55;
                color = mix(color, vec3(0.95, 0.05, 0.12), uDamage * 0.5 * (1.0 - vignette * 0.5));

                float gray = dot(color, vec3(0.299, 0.587, 0.114));
                color = mix(color, vec3(gray) * vec3(0.95, 0.88, 1.0), uDesaturate);

                color *= 0.965 + 0.035 * sin(uv.y * uResolution.y * 3.14159);
                color += (Hash(uv * uResolution + fract(uTime) * 97.0) - 0.5) * 0.035;
                color *= 1.0 - uFade;
                outColor = vec4(color, 1.0);
            }
        )";

        [[nodiscard]] float Approach(const float value, const float target, const float rate, const float deltaTime) {
            return value + (target - value) * std::min(1.0f, rate * deltaTime);
        }
    }

    void ScreenFx::Install(SpinoCore::Graphics::PostProcessManager& postProcess) {
        SpinoCore::Graphics::PostProcess effect{
            .fragmentShader = Shader,
            .uniforms = {
                PostProcessUniform{.type = UniformType::Float, .value = 0.0f},
                PostProcessUniform{.type = UniformType::Float, .value = 0.0f},
                PostProcessUniform{.type = UniformType::Float, .value = 0.0f},
                PostProcessUniform{.type = UniformType::Float, .value = 0.0f},
                PostProcessUniform{.type = UniformType::Float, .value = 1.0f},
                PostProcessUniform{.type = UniformType::Vec4, .value = SpinoCore::Math::Vector4f{0.0f, 0.0f, 0.0f, 0.0f}},
            },
        };
        if (!postProcess.Contains(Config::PostProcessName)) postProcess.AddPostProcess(Config::PostProcessName, std::move(effect));
        postProcess.SetCurrent(Config::PostProcessName);
        if (!postProcess.IsActive()) postProcess.ToggleActive();
    }

    void ScreenFx::KeepSharp(const SpinoCore::Utils::Size2D screen, const SpinoCore::Math::Vector2f center, const SpinoCore::Math::Vector2f size) {
        const auto width = static_cast<float>(screen.width);
        const auto height = static_cast<float>(screen.height);
        const float left = center.x - size.x * 0.5f - SharpMargin;
        const float right = center.x + size.x * 0.5f + SharpMargin;
        const float top = center.y - size.y * 0.5f - SharpMargin;
        const float bottom = center.y + size.y * 0.5f + SharpMargin;
        mSharpRect = SpinoCore::Math::Vector4f{left / width, 1.0f - bottom / height, right / width, 1.0f - top / height};
    }

    void ScreenFx::Pulse(const float strength) {
        mPulse = std::max(mPulse, strength);
    }

    void ScreenFx::Damage(const float strength) {
        mDamage = std::max(mDamage, strength);
    }

    void ScreenFx::Update(const float deltaTime, SpinoCore::Graphics::PostProcessManager& postProcess) {
        mPulse = std::max(0.0f, mPulse - deltaTime * 3.5f);
        mDamage = std::max(0.0f, mDamage - deltaTime * 2.2f);
        mBlur = Approach(mBlur, mBlurTarget, 8.0f, deltaTime);
        mDesaturate = Approach(mDesaturate, mDesaturateTarget, 2.5f, deltaTime);
        mFade = Approach(mFade, mFadeTarget, 3.0f, deltaTime);

        postProcess.SetUniform(Config::PostProcessName, PulseSlot, mPulse);
        postProcess.SetUniform(Config::PostProcessName, DamageSlot, mDamage);
        postProcess.SetUniform(Config::PostProcessName, BlurSlot, mBlur);
        postProcess.SetUniform(Config::PostProcessName, DesaturateSlot, mDesaturate);
        postProcess.SetUniform(Config::PostProcessName, FadeSlot, mFade);
        postProcess.SetUniform(Config::PostProcessName, SharpSlot, mSharpRect);
    }
}
