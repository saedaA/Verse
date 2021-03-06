#pragma once

#include <glm/glm.hpp>

#include <Deliberation/ECS/Entity.h>

#include "GameLib.h"
#include "NpcTask.h"

class NpcAttackTask : public NpcTask
{
public:
    NpcAttackTask();

    const Entity & target() const;

    void setTarget(Entity target);

    void update(
        NpcController & controller,
        RigidBody &     body,
        Equipment &     equipment,
        const UpdateFrame & updateFrame) override;

private:
    enum class Status
    {
        None,
        Joust,
        Evade
    };

private:
    void startEvasion(RigidBody & body, NpcController & controller);
    void startJoust();

private:
    Entity    m_target;
    Status    m_status = Status::None;
    glm::vec3 m_evasionPoint;
};