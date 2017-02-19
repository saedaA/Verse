#pragma once

#include <Deliberation/Core/Optional.h>
#include <Deliberation/Physics/PhysicsWorld.h>

#include "Hailstorm/HailstormParticle.h"
#include "Hailstorm/HailstormRenderer.h"
#include "Hailstorm/HailstormPhysicsWorld.h"

class VoxelWorld;

class HailstormManager final
{
public:
    HailstormManager(Context & context, const Camera3D & camera, PhysicsWorld & physicsWorld, VoxelWorld & voxelWorld);

    HailstormRenderer & renderer();
    const HailstormRenderer & renderer() const;

    void addBullet(HailstormParticle bullet);
    void addParticle(HailstormParticle bullet);

    void update(float seconds);
    void render();

private:
    VoxelWorld &                    m_voxelWorld;
    Optional<HailstormRenderer>     m_renderer;
    Optional<HailstormPhysicsWorld> m_physicsWorld;
};
