#include <iostream>
#include <thread>

#include <Deliberation/Core/Optional.h>
#include <Deliberation/Physics/PhysicsWorld.h>
#include <Deliberation/Platform/Application.h>
#include <Deliberation/Scene/Camera3D.h>
#include <Deliberation/Scene/UVSphere.h>
#include <Deliberation/Scene/Debug/DebugCameraNavigator3D.h>
#include <Deliberation/Scene/Debug/DebugGroundPlaneRenderer.h>

#include "HudButton.h"
#include "HudEntityMarkersRenderer.h"
#include "ResourceManager.h"
#include "VerseApplication.h"

using namespace deliberation;

class HudSandbox:
    public VerseApplication
{
public:
    HudSandbox():
        VerseApplication("HudSandbox", VerseApplicationSystemInitMode::NoSystems)
    {
    }

    void onApplicationStartup() override
    {
        EnableGLErrorChecksAndLogging();

        auto resourceManager = m_world.addSystem<ResourceManager>();

        m_hudEntityMarkersRenderer = std::make_shared<HudEntityMarkersRenderer>(context(), *resourceManager);

        {
            auto button = std::make_shared<HudButton>();
            button->setPosition({0.0f, 0.0f});
            button->setHalfExtent({0.2f, 0.2f});
            m_entityMarkers.emplace_back(button);
        }

        {
            auto button = std::make_shared<HudButton>();
            button->setPosition({0.2f, 0.3f});
            button->setHalfExtent({0.1f, 0.1f});
            m_entityMarkers.emplace_back(button);
        }
    }

    void onApplicationUpdate(float seconds) override
    {
    }

    void onApplicationRender() override
    {
        m_hudEntityMarkersRenderer->render(m_entityMarkers);
    }

private:

    std::shared_ptr<HudEntityMarkersRenderer>   m_hudEntityMarkersRenderer;
    std::vector<std::shared_ptr<HudButton>>     m_entityMarkers;
};

int main(int argc, char *argv[])
{
    return HudSandbox().run(argc, argv);
}