#pragma once

#include <stack>

#include <Deliberation/Core/LayoutedBlob.h>
#include <Deliberation/Core/LinearMap.h>

#include <Deliberation/Draw/Buffer.h>
#include <Deliberation/Draw/Draw.h>
#include <Deliberation/Draw/Program.h>

#include <Deliberation/Scene/Mesh2.h>

#include "VfxParticle.h"

#include "GameLib.h"

class VfxRenderer;

class VfxRenderBatch
{
public:
    VfxRenderBatch(VfxRenderer & renderer, const Mesh2 & mesh);

    size_t addInstance(const VfxParticle & particle);
    void removeInstance(size_t index);

    void render();

private:
    void addInstanceInSlot(const VfxParticle & bullet, size_t index);

private:
    VfxRenderer &       m_renderer;

    Draw                m_draw;
    Buffer              m_instanceBuffer;
    Buffer              m_vertexBuffer;
    Buffer              m_indexBuffer;

    LayoutedBlob        m_instances;
    TypedBlobValueAccessor<glm::vec3>
                        m_origins;
    TypedBlobValueAccessor<glm::vec3>
                        m_velocities;
    TypedBlobValueAccessor<u32>
                        m_lifetimes;
    TypedBlobValueAccessor<u32>
                        m_births;

    std::stack<size_t>  m_freeInstanceSlots;

    LinearMap<size_t>   m_instanceIndexByBulletID;
};