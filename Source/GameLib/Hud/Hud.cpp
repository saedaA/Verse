#include "Hud.h"

#include <algorithm>

#include <Deliberation/ECS/Systems/PhysicsWorldSystem.h>
#include <Deliberation/ECS/Phase.h>
#include <Deliberation/ECS/World.h>

#include <Deliberation/Scene/Pipeline/RenderManager.h>
#include <Deliberation/Platform/App.h>

#include "HudButton.h"
#include "HudCrosshairs.h"
#include "HudRenderer.h"
#include "PlayerSystem.h"

Hud::Hud(World & world)
    : Base(world)
    , InputLayer(1)
    , m_playerSystem(world.systemRef<PlayerSystem>())
{
    auto & physicsWorld = world.systemRef<PhysicsWorldSystem>().physicsWorld();

    auto crosshairs = std::make_shared<HudCrosshairs>(*this);

    GetGlobal<RenderManager>()->addRenderer<HudRenderer>(*this);

    m_layers.emplace_back(crosshairs);
    m_layers.emplace_back(std::make_shared<HudEntityMarkers>(
        *this, physicsWorld, GetGlobal<RenderManager>()->mainCamera()));

    activatePhases<GameUpdatePhase>();

    addElement(crosshairs);
}

void Hud::setPlayerTarget(Entity & entity)
{
    m_playerSystem.setPlayerTarget(entity);
}

void Hud::addElement(const std::shared_ptr<HudElement> & element)
{
    m_elements.emplace_back(element);
}

void Hud::removeElement(const std::shared_ptr<HudElement> & element)
{
    const auto iter = std::find(m_elements.begin(), m_elements.end(), element);
    AssertM(iter != m_elements.end(), "No such button");

    m_elements.erase(iter);
}

void Hud::onGameUpdate(const UpdateFrame & updateFrame)
{
    for (auto & layer : m_layers)
    {
        layer->update(updateFrame);
    }
}

void Hud::onMouseButtonClicked(MouseButtonEvent & event)
{
    processMouseEvent(event, [&](HudElement & element) {
        element.onMouseButtonClicked(event);
    });
}

void Hud::onMouseButtonPressed(MouseButtonEvent & event)
{
    processMouseEvent(event, [&](HudElement & element) {
        element.onMouseButtonPressed(event);
    });
}

template<typename T>
void Hud::processMouseEvent(
    T & event, const std::function<void(HudElement & element)> & fn) const
{
    for (auto & element : m_elements)
    {
        if (!element->visible()) continue;

        const auto & halfExtent = element->halfExtent();
        const auto   absMouseDelta =
            glm::abs(event.mousePosition() - element->position());

        if (absMouseDelta.x <= halfExtent.x && absMouseDelta.y <= halfExtent.y)
        {
            fn(*element);
            if (event.isConsumed()) break;
        }
    }
}