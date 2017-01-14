#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Voxel.h"
#include "VoxelRenderChunk.h"

namespace deliberation
{
class Pose3D;
}

class VoxelWorld;

class VoxelRenderChunkTree final {
public:
    VoxelRenderChunkTree(const glm::uvec3 & size);

    const glm::uvec3 & size() const { return m_size; }

    void addVoxels(const std::vector<Voxel> & voxels);
    void removeVoxel(const std::vector<glm::uvec3> & voxels);

    void schedule(const VoxelWorld & voxelWorld, const Pose3D & pose);

    std::string toString() const;

protected:
    struct Node
    {
        glm::ivec3  llf;
        glm::ivec3  urb;
        bool        empty = true;
        bool        hidden = true;
        size_t      chunk = std::numeric_limits<size_t>::max();
        bool        leaf = false;
    };

    struct ChunkWrapper
    {
        glm::vec3   position;
        std::shared_ptr<VoxelRenderChunk>
                    chunk;
    };

private:
    void addVoxelToNode(u32 index, const Voxel & voxel);
    void removeVoxelFromNode(u32 index, const glm::uvec3 & voxel);

protected:
    glm::uvec3          m_size;
    std::vector<bool>   m_nodeMask;
    std::vector<Node>   m_nodes;
    std::vector<ChunkWrapper>
                        m_chunks;
};