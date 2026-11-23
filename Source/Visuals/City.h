#pragma once
#include <vector>

namespace SpinoCore::Core::Actors::Components {
    class ModelComponent;
}

namespace Nightmare {
    class ActorRegistry;
    class ScrollActor;
    struct WorldMotion;

    class City final {
    public:
        City(ActorRegistry& registry, const WorldMotion& motion);

        void Build();
        void Recycle();

    private:
        struct Building final {
            ScrollActor* actor{nullptr};
            SpinoCore::Core::Actors::Components::ModelComponent* model{nullptr};
            float halfLength{1.0f};
            int side{1};
        };

        void BuildRoad();
        void BuildBuildings(int side);
        void BuildLamps(int side);
        void Dress(Building& building, float frontZ);
        [[nodiscard]] float BackmostEdge(int side) const;

        ActorRegistry& mRegistry;
        const WorldMotion& mMotion;
        std::vector<ScrollActor*> mRoad;
        std::vector<Building> mBuildings;
        std::vector<ScrollActor*> mLamps;
    };
}
