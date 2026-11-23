#pragma once
#include "SpinoCore/Sound/Synth/SynthUtils.h"
#include "SpinoCore/Sound/Synth/Timbre.h"

namespace Nightmare {
    class LeadTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Sawtooth;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.oscMix = 0.3f;
            mConfig.detune = 2.0f;
            mConfig.filterCutoff = 3800.0f;
            mConfig.attack = 0.008f;
            mConfig.decay = 0.35f;
            mConfig.sustain = 0.55f;
            mConfig.release = 0.5f;
            mConfig.delayTime = 0.3461f;
            mConfig.delayFeedback = 0.38f;
            mConfig.delayMix = 0.28f;
            mConfig.reverbSize = 0.85f;
            mConfig.reverbMix = 0.3f;
            mConfig.volume = 2.0f;
        }
    };

    class SubTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Triangle;
            mConfig.oscMix = 0.5f;
            mConfig.detune = 0.6f;
            mConfig.filterCutoff = 1800.0f;
            mConfig.attack = 0.005f;
            mConfig.decay = 0.25f;
            mConfig.sustain = 0.5f;
            mConfig.release = 0.35f;
            mConfig.reverbSize = 0.5f;
            mConfig.reverbMix = 0.15f;
            mConfig.volume = 1.0f;
        }
    };

    class PadTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Sawtooth;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Triangle;
            mConfig.oscMix = 0.5f;
            mConfig.detune = 1.5f;
            mConfig.filterCutoff = 1400.0f;
            mConfig.attack = 0.35f;
            mConfig.decay = 0.6f;
            mConfig.sustain = 0.75f;
            mConfig.release = 0.9f;
            mConfig.reverbSize = 0.75f;
            mConfig.reverbMix = 0.35f;
            mConfig.volume = 1.0f;
        }
    };

    class BassTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Sawtooth;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.oscMix = 0.35f;
            mConfig.detune = 0.4f;
            mConfig.filterCutoff = 900.0f;
            mConfig.attack = 0.003f;
            mConfig.decay = 0.12f;
            mConfig.sustain = 0.55f;
            mConfig.release = 0.06f;
            mConfig.volume = 1.0f;
        }
    };

    class KickTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Sine;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Triangle;
            mConfig.oscMix = 0.25f;
            mConfig.attack = 0.001f;
            mConfig.decay = 0.22f;
            mConfig.sustain = 0.0f;
            mConfig.release = 0.05f;
            mConfig.volume = 1.0f;
        }
    };

    class SnareTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Triangle;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.oscMix = 0.55f;
            mConfig.detune = 4.0f;
            mConfig.filterCutoff = 7000.0f;
            mConfig.attack = 0.001f;
            mConfig.decay = 0.16f;
            mConfig.sustain = 0.0f;
            mConfig.release = 0.08f;
            mConfig.reverbSize = 0.7f;
            mConfig.reverbMix = 0.3f;
            mConfig.volume = 1.0f;
        }
    };

    class HatTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.detune = 6.0f;
            mConfig.attack = 0.001f;
            mConfig.decay = 0.045f;
            mConfig.sustain = 0.0f;
            mConfig.release = 0.02f;
            mConfig.volume = 1.0f;
        }
    };

    class ZapTimbre final : public SpinoCore::Sound::Synth::Timbre {
    public:
        void SetupInstrument() override {
            mConfig.osc1Type = SpinoCore::Sound::Synth::WaveformType::Square;
            mConfig.osc2Type = SpinoCore::Sound::Synth::WaveformType::Sawtooth;
            mConfig.oscMix = 0.5f;
            mConfig.detune = 3.0f;
            mConfig.filterCutoff = 5000.0f;
            mConfig.attack = 0.001f;
            mConfig.decay = 0.07f;
            mConfig.sustain = 0.0f;
            mConfig.release = 0.04f;
            mConfig.delayTime = 0.115f;
            mConfig.delayFeedback = 0.25f;
            mConfig.delayMix = 0.2f;
            mConfig.volume = 1.0f;
        }
    };
}
