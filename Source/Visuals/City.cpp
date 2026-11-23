#include "City.h"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include "Actors/ActorRegistry.h"
#include "Actors/ModelActor.h"
#include "Actors/ScrollActor.h"
#include "Catalog.h"
#include "Config.h"
#include "Effects/GlowEffect.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/Components/ModelComponent.h"
#include "SpinoCore/Core/Rendering/BuiltinModels.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Math/Quaternion.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Random.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Core::Actors::ActorInput;
        using SpinoCore::Core::Actors::Components::ModelComponent;
        using SpinoCore::Math::Quaternionf;
        using SpinoCore::Math::Vector3f;
        using SpinoCore::Math::Vector4f;
        namespace Random = SpinoCore::Utils::Random;

        constexpr float TileLength = 8.0f;
        constexpr float StreetFrontZ = Config::RecycleZ + 6.0f;
        constexpr float BuildingSetback = 6.2f;
        constexpr float LampSpacing = 20.0f;
        constexpr float LampX = 4.35f;
        constexpr float PoleHeight = 5.2f;

        [[nodiscard]] Quaternionf Yaw(const float degrees) {
            return Quaternionf{Vector3f::UnitY(), SpinoCore::Math::Operations::ToRadians(degrees)};
        }

        [[nodiscard]] const Catalog::BuildingSpec& RandomBuilding() {
            return Catalog::Buildings[static_cast<std::size_t>(Random::GetIntRange(0, static_cast<int>(Catalog::Buildings.size()) - 1))];
        }
    }

    City::City(ActorRegistry& registry, const WorldMotion& motion) : mRegistry(registry), mMotion(motion) {}

    void City::Build() {
        auto ground = std::make_unique<ModelActor>(ActorInput{
            .position = Vector3f{0.0f, -0.16f, -60.0f},
            .scale = Vector3f{14.0f, 1.0f, 30.0f},
        }, "ground");
        mRegistry.Spawn(std::move(ground));

        BuildRoad();
        BuildBuildings(-1);
        BuildBuildings(1);
        BuildLamps(-1);
        BuildLamps(1);
    }

    void City::BuildRoad() {
        const auto count = static_cast<std::size_t>((StreetFrontZ + Config::WorldDepth) / TileLength) + 2;
        for (std::size_t index = 0; index < count; ++index) {
            const float z = StreetFrontZ - TileLength * 0.5f - TileLength * static_cast<float>(index);
            auto tile = std::make_unique<ScrollActor>(ActorInput{.position = Vector3f{0.0f, -0.1f, z}}, mMotion);
            tile->AddComponent(std::make_unique<ModelComponent>(*tile, Config::StreetModel));
            mRoad.push_back(mRegistry.Spawn(std::move(tile)));
        }
    }

    void City::BuildBuildings(const int side) {
        float frontZ = StreetFrontZ;
        while (frontZ > -Config::WorldDepth - 20.0f) {
            auto actor = std::make_unique<ScrollActor>(ActorInput{}, mMotion);
            auto model = std::make_unique<ModelComponent>(*actor, std::string(Catalog::Buildings.front().model));
            Building building{.actor = actor.get(), .model = model.get(), .side = side};
            actor->AddComponent(std::move(model));
            mRegistry.Spawn(std::move(actor));
            Dress(building, frontZ);
            frontZ = building.actor->GetPosition().z - building.halfLength - Random::GetFloatRange(0.4f, 2.5f);
            mBuildings.push_back(building);
        }
    }

    void City::Dress(Building& building, const float frontZ) {
        const Catalog::BuildingSpec& spec = RandomBuilding();
        const float scale = Random::GetFloatRange(3.6f, 4.6f);
        const float length = spec.size.x * scale;
        const float depth = spec.size.z * scale;
        building.halfLength = length * 0.5f;
        building.model->SetModelId(spec.model);
        building.actor->SetScale(Vector3f{scale, scale * Random::GetFloatRange(0.9f, 1.25f), scale});
        building.actor->SetRotation(Yaw(building.side < 0 ? 90.0f : -90.0f));
        const float x = static_cast<float>(building.side) * (BuildingSetback + depth * 0.5f + Random::GetFloatRange(0.0f, 1.2f));
        building.actor->SetPosition(Vector3f{x, 0.0f, frontZ - building.halfLength});
    }

    void City::BuildLamps(const int side) {
        const auto count = static_cast<std::size_t>((StreetFrontZ + Config::WorldDepth) / LampSpacing) + 1;
        const float sideSign = static_cast<float>(side);
        for (std::size_t index = 0; index < count; ++index) {
            const float z = StreetFrontZ - LampSpacing * static_cast<float>(index) - (side > 0 ? LampSpacing * 0.5f : 0.0f);
            auto root = std::make_unique<ScrollActor>(ActorInput{.position = Vector3f{sideSign * LampX, 0.0f, z}}, mMotion);
            const Vector4f color = index % 2 == 0 ? Vector4f{1.0f, 0.3f, 0.85f, 0.9f} : Vector4f{0.3f, 0.85f, 1.0f, 0.9f};
            root->AddComponent(std::make_unique<GlowEffect>(*root, std::vector<GlowPoint>{
                GlowPoint{.offset = Vector3f{-sideSign * 1.05f, PoleHeight - 0.12f, 0.0f}, .size = 0.9f, .color = Vector4f{color.x, color.y, color.z, 0.55f}, .flicker = 0.08f},
                GlowPoint{.offset = Vector3f{-sideSign * 1.05f, PoleHeight - 0.12f, 0.0f}, .size = 0.28f, .color = Vector4f{1.0f, 1.0f, 1.0f, 0.95f}, .flicker = 0.0f},
            }));
            ScrollActor* lamp = mRegistry.Spawn(std::move(root));

            auto pole = std::make_unique<ModelActor>(ActorInput{
                .position = Vector3f{0.0f, PoleHeight * 0.5f, 0.0f},
                .scale = Vector3f{0.14f, PoleHeight, 0.14f},
            }, std::string(SpinoCore::Core::Rendering::CubeModelId));
            mRegistry.Spawn(std::move(pole), lamp);

            auto arm = std::make_unique<ModelActor>(ActorInput{
                .position = Vector3f{-sideSign * 0.55f, PoleHeight, 0.0f},
                .scale = Vector3f{1.2f, 0.1f, 0.12f},
            }, std::string(SpinoCore::Core::Rendering::CubeModelId));
            mRegistry.Spawn(std::move(arm), lamp);
            mLamps.push_back(lamp);
        }
    }

    float City::BackmostEdge(const int side) const {
        float edge = std::numeric_limits<float>::max();
        for (const Building& building : mBuildings) {
            if (building.side == side) edge = std::min(edge, building.actor->GetPosition().z - building.halfLength);
        }
        return edge;
    }

    void City::Recycle() {
        const float roadLength = TileLength * static_cast<float>(mRoad.size());
        for (ScrollActor* tile : mRoad) {
            auto position = tile->GetPosition();
            if (position.z - TileLength * 0.5f > StreetFrontZ) {
                position.z -= roadLength;
                tile->SetPosition(position);
            }
        }

        for (Building& building : mBuildings) {
            if (building.actor->GetPosition().z - building.halfLength <= StreetFrontZ) continue;
            Dress(building, BackmostEdge(building.side) - Random::GetFloatRange(0.4f, 2.5f));
        }

        const float lampsLength = LampSpacing * static_cast<float>(mLamps.size() / 2);
        for (ScrollActor* lamp : mLamps) {
            auto position = lamp->GetPosition();
            if (position.z > StreetFrontZ) {
                position.z -= lampsLength;
                lamp->SetPosition(position);
            }
        }
    }
}
