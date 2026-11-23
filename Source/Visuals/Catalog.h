#pragma once
#include <array>
#include "SpinoCore/Math/Vector.h"

namespace Nightmare::Catalog {
    using SpinoCore::Math::Vector3f;

    struct BuildingSpec final {
        const char* model;
        Vector3f size;
    };

    struct ObstacleSpec final {
        const char* model;
        float scale;
        Vector3f offset;
        Vector3f halfSize;
        float yawDegrees;
    };

    inline constexpr std::array Buildings{
        BuildingSpec{"building0", Vector3f{1.24f, 1.68f, 1.24f}}, BuildingSpec{"building1", Vector3f{1.24f, 2.08f, 1.34f}},
        BuildingSpec{"building2", Vector3f{0.84f, 1.68f, 0.84f}}, BuildingSpec{"building3", Vector3f{2.32f, 2.48f, 1.82f}},
        BuildingSpec{"building4", Vector3f{2.04f, 1.68f, 1.24f}}, BuildingSpec{"building5", Vector3f{2.04f, 1.5f, 0.85f}},
        BuildingSpec{"building6", Vector3f{0.97f, 1.68f, 0.84f}}, BuildingSpec{"building7", Vector3f{0.5f, 1.98f, 0.5f}},
        BuildingSpec{"building8", Vector3f{0.5f, 2.25f, 0.5f}}, BuildingSpec{"building9", Vector3f{0.5f, 2.0f, 0.5f}},
        BuildingSpec{"building10", Vector3f{0.5f, 1.75f, 0.5f}}, BuildingSpec{"building11", Vector3f{0.5f, 2.1f, 0.5f}},
        BuildingSpec{"building12", Vector3f{0.5f, 2.23f, 0.5f}}, BuildingSpec{"building13", Vector3f{0.5f, 1.75f, 0.5f}},
        BuildingSpec{"building14", Vector3f{0.5f, 1.78f, 0.5f}}, BuildingSpec{"building15", Vector3f{0.5f, 1.85f, 0.5f}},
        BuildingSpec{"building16", Vector3f{1.0f, 1.1f, 0.5f}}, BuildingSpec{"building17", Vector3f{1.0f, 1.15f, 0.5f}},
        BuildingSpec{"building18", Vector3f{1.24f, 5.5f, 1.37f}}, BuildingSpec{"building19", Vector3f{1.24f, 4.08f, 1.37f}},
        BuildingSpec{"building20", Vector3f{1.24f, 4.08f, 1.24f}}, BuildingSpec{"building21", Vector3f{1.24f, 2.88f, 1.24f}},
        BuildingSpec{"building22", Vector3f{1.24f, 3.15f, 1.24f}}, BuildingSpec{"building23", Vector3f{1.24f, 4.48f, 1.24f}},
        BuildingSpec{"building24", Vector3f{0.84f, 0.88f, 0.94f}}, BuildingSpec{"building25", Vector3f{0.84f, 1.28f, 0.97f}},
        BuildingSpec{"building26", Vector3f{1.64f, 0.88f, 0.97f}}, BuildingSpec{"building27", Vector3f{0.84f, 1.28f, 0.94f}},
        BuildingSpec{"building28", Vector3f{0.84f, 1.28f, 0.84f}}, BuildingSpec{"building29", Vector3f{0.84f, 1.28f, 0.94f}},
    };

    inline constexpr std::array Obstacles{
        ObstacleSpec{"cone", 2.2f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{0.42f, 0.5f, 0.42f}, 0.0f},
        ObstacleSpec{"concrete", 1.5f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{0.9f, 0.47f, 0.22f}, 0.0f},
        ObstacleSpec{"barrier", 0.5f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{0.92f, 0.8f, 0.17f}, 0.0f},
        ObstacleSpec{"trafficBarrier", 0.21f, Vector3f{0.0f, 0.81f, 0.0f}, Vector3f{0.88f, 0.7f, 0.5f}, 0.0f},
        ObstacleSpec{"spikeMine", 6.0f, Vector3f{0.0f, 0.36f, 0.0f}, Vector3f{0.45f, 0.36f, 0.45f}, 0.0f},
        ObstacleSpec{"policeCar", 1.0f, Vector3f{0.0f, 0.02f, 0.0f}, Vector3f{0.86f, 0.62f, 1.85f}, 0.0f},
        ObstacleSpec{"car", 0.04f, Vector3f{0.0f, 0.0f, 0.02f}, Vector3f{0.88f, 0.64f, 1.75f}, 0.0f},
        ObstacleSpec{"pickup", 0.012f, Vector3f{0.066f, -0.164f, 2.13f}, Vector3f{0.8f, 0.72f, 1.9f}, 0.0f},
    };
}
