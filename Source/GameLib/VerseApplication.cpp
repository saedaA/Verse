#include "VerseApplication.h"

#include <Deliberation/Draw/TextureLoader.h>
#include <Npc/NpcBehaviourSystem.h>

#include "VoxelClusterSplitSystem.h"

#include "EntityPrototypeManager.h"
#include "EquipmentSystem.h"
#include "FactionManager.h"
#include "NpcBehaviourSystem.h"
#include "PlayerSystem.h"
#include "ResourceManager.h"
#include "CoriolisSystem.h"
#include "VfxSystem.h"

VerseApplication::VerseApplication(const std::string & name):
    Application(name)
{

}

void VerseApplication::onStartup()
{
    m_physicsWorld.narrowphase().registerPrimitiveTest((int)::CollisionShapeType::VoxelCluster,
                                                       std::make_unique<VoxelClusterPrimitiveTest>());

    m_physicsWorld.narrowphase().contactDispatcher().
        registerContactType<VoxelClusterContact>((int)::CollisionShapeType::VoxelCluster);

    m_camera.setPosition({0.0f, 200.0f, 300.0f});
    m_camera.setOrientation(glm::quat({-1.0f, 0.0f, 0.0f}));
    m_camera.setAspectRatio((float)context().backbuffer().width() / context().backbuffer().height());

    auto skyboxPaths = std::array<std::string, 6> {
        GameDataPath("Data/Skybox/Right.png"),
        GameDataPath("Data/Skybox/Left.png"),
        GameDataPath("Data/Skybox/Top.png"),
        GameDataPath("Data/Skybox/Bottom.png"),
        GameDataPath("Data/Skybox/Front.png"),
        GameDataPath("Data/Skybox/Back.png")
    };

    auto skyboxCubemapBinary = TextureLoader(skyboxPaths).load();
    auto skyboxCubemap = context().createTexture(skyboxCubemapBinary);

    m_skyboxRenderer = std::make_shared<SkyboxRenderer>(context(), m_camera, skyboxCubemap);

    m_clear = context().createClear();

    m_groundPlane.reset(context(), m_camera);
    m_groundPlane->setSize(1000.0f);
    m_groundPlane->setQuadSize(100.0f);
    m_groundPlane->setRadius(750.0f);

    m_world.addSystem<ResourceManager>(context());
    m_world.addSystem<PhysicsWorldSystem>(m_physicsWorld);
    m_world.addSystem<VoxelClusterSplitSystem>();
    m_voxelWorld = m_world.addSystem<VoxelWorld>(context(), m_physicsWorld, m_camera, skyboxCubemap);
    m_world.addSystem<NpcControllerSystem>();
    m_hailstormManager = m_world.addSystem<HailstormManager>(context(), m_camera, m_physicsWorld, *m_voxelWorld);
    m_vfxManager.emplace(context(), m_camera, *m_voxelWorld);
    m_world.addSystem<VfxSystem>(*m_vfxManager);
    m_debugOverlay = m_world.addSystem<DebugOverlay>(context());
    m_world.addSystem<CoriolisSystem>();
    m_world.addSystem<EquipmentSystem>();
    m_world.addSystem<PlayerSystem>(context(), input(), m_camera, m_physicsWorld);
    m_world.addSystem<FactionManager>();
    m_world.addSystem<NpcBehaviourSystem>();

    m_debugGeometryManager.emplace(context());

    m_entityPrototypeManager = std::make_shared<EntityPrototypeManager>(m_world);

    onApplicationStartup();
}

void VerseApplication::onFrame(float seconds)
{
    m_debugOverlay->setFps(fps());

    auto physicsSimulationSeconds = m_physicsWorld.nextSimulationStepSeconds(seconds);

    if (EpsilonGt(physicsSimulationSeconds, 0.0f))
    {
        m_world.prePhysicsUpdate(physicsSimulationSeconds);
        m_physicsWorld.update(seconds);
        m_hailstormManager->update(physicsSimulationSeconds);
        m_vfxManager->update(physicsSimulationSeconds);

        onApplicationPhysicsUpdate(physicsSimulationSeconds);
    }
    else
    {
        m_physicsWorld.update(seconds);
    }

    onApplicationUpdate(seconds);

    m_world.update(seconds);

    m_clear.render();
    m_world.render();
  //  m_groundPlane->render();
    m_skyboxRenderer->render();

    onApplicationRender();

    m_hailstormManager->render();
    m_vfxManager->render();

    m_world.system<PlayerSystem>().renderUi();

    m_world.frameComplete();
}