#include <iostream>
#include <memory>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Deliberation/Core/Math/FloatUtils.h>
#include <Deliberation/Core/Math/Random.h>
#include <Deliberation/Core/Optional.h>

#include <Deliberation/ECS/Entity.h>
#include <Deliberation/ECS/Systems/PhysicsWorldSystem.h>
#include <Deliberation/ECS/World.h>

#include <Deliberation/Physics/PrimitiveTester.h>
#include <Deliberation/Physics/PhysicsWorld.h>
#include <Deliberation/Physics/RigidBody.h>

#include <Deliberation/Platform/App.h>

#include <Deliberation/Scene/Camera3D.h>
#include <Deliberation/Scene/CameraDolly3D.h>
#include <Deliberation/Scene/Debug/DebugCameraNavigator3D.h>
#include <Deliberation/Scene/Debug/DebugGeometryNode.h>
#include <Deliberation/Scene/Debug/DebugGeometryRenderer.h>
#include <Deliberation/Scene/Debug/DebugGroundPlaneRenderer.h>
#include <Deliberation/Scene/UVSphere.h>
#include <Npc/NpcDebugRendererSystem.h>

#include "AimHelper.h"
#include "CollisionShapeTypes.h"
#include "Components.h"
#include "Emitter.h"
#include "Equipment.h"
#include "Hardpoint.h"
#include "NpcAttackTask.h"
#include "NpcController.h"
#include "NpcControllerSystem.h"
#include "NpcFlightControl.h"
#include "NpcSteering.h"
#include "PlayerFlightControl.h"
#include "Deliberation/Resource/ResourceManager.h"
#include "VerseRuntime.h"
#include "VfxManager.h"
#include "VoxReader.h"
#include "VoxelClusterContact.h"
#include "VoxelClusterPrimitiveTest.h"
#include "VoxelObjectPrimitives.h"
#include "VoxelRenderable.h"
#include "VoxelRigidBodyPayload.h"
#include "VoxelWorld.h"
#include "Weapon.h"

#include "SandboxApplication.h"

using namespace deliberation;

class ShootingRangeSandbox : public VerseRuntime
{
public:
    ShootingRangeSandbox()
        : VerseRuntime(
              "ShootingRangeSandbox", VerseApplicationSystemInitMode::NoSystems)
    {
    }

    void onApplicationStartup() override
    {
        m_world.addSystem<ResourceManager>();
        m_world.addSystem<PhysicsWorldSystem>(m_physicsWorld);
        m_world.addSystem<VoxelWorld>(m_camera, m_skyboxCubemap);
        m_world.addSystem<HailstormManager>(m_camera);
        auto hailstormManager = m_world.addSystem<HailstormManager>(m_camera);

        auto bulletMesh = UVSphere(5, 5).generateMesh2();
        m_bulletMeshID =
            hailstormManager->vfxManager().meshRenderer().addMesh(bulletMesh);

        WeaponConfig weaponConfig;
        weaponConfig.cooldown = 0.1f;
        weaponConfig.meshID = m_bulletMeshID;

        HardpointDesc hardpointDesc;
        hardpointDesc.maxAngle = glm::half_pi<float>();

        m_hardpoint = std::make_shared<Hardpoint>(hardpointDesc);
        m_hardpoint->setWeapon(std::make_shared<Weapon>(
            weaponConfig, *hailstormManager, INVALID_VOXEL_OBJECT_WORLD_UID));
    }

    void onApplicationUpdate(float seconds) override
    {
        if (input().mouseButtonDown(MouseButton::Right))
        {
            AimHelper aimHelper(m_camera, m_physicsWorld);
            auto      hit = false;
            auto      result = aimHelper.getTarget(input().mousePosition());

            m_hardpoint->setFireRequest(
                true,
                glm::normalize(result.pointOfImpact - m_camera.position()));
        }
        else
        {
            m_hardpoint->setFireRequest(false, {});
        }

        EquipmentUpdateContext context;
        context.targetPose =
            m_camera.pose().localTranslated({5.0f, 0.0f, -15.0f});

        m_hardpoint->update(seconds, context);
    }

    void onApplicationRender() override {}

private:
    std::shared_ptr<Hardpoint> m_hardpoint;
    VfxMeshId                  m_bulletMeshID;
};

int main(int argc, char * argv[])
{
    return ShootingRangeSandbox().run(argc, argv);
}
