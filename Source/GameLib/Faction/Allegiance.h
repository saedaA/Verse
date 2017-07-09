#pragma once

#include <Deliberation/ECS/Component.h>
#include <Deliberation/ECS/World.h>

#include "GameLib.h"
#include "FactionChangeEvent.h"

class Allegiance:
    public Component<Allegiance>
{
public:
    const std::string & faction() const;
    void setFaction(const std::string & faction) {
        Assert(!faction.empty(), "Empty faction only valid on initialization");

        World * world = (World*)m_world;
        FactionChangeEvent event(world->entityById(m_entityId), m_faction, faction);
        m_faction = faction;

        publishEvent(event);
    }

private:
    std::string m_faction;
};