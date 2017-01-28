#pragma once

#include <glm/glm.hpp>

#include "GameLib.h"
#include "Voxel.h"
#include "VoxelHull.h"
#include "VoxelRenderChunkTree.h"
#include "VoxelShape.h"

class VoxelWorld;

class VoxelObjectVoxelData final
{
public:
    VoxelObjectVoxelData(const VoxelWorld & voxelWorld, const glm::uvec3 & size);

    const VoxelWorld & voxelWorld() const;
    const glm::uvec3 & size() const;
    const VoxelCluster<bool> & cluster() const;
    const VoxelRenderChunkTree & renderTree() const;
    const std::shared_ptr<VoxelShape> & shapeTree() const;
    const VoxelHull & hull() const;

    void addVoxels(std::vector<Voxel> voxels);
    void removeVoxels(const std::vector<glm::uvec3> & voxels);

private:
    const VoxelWorld &              m_voxelWorld;
    VoxelCluster<bool>              m_cluster;
    VoxelRenderChunkTree            m_renderTree;
    std::shared_ptr<VoxelShape>     m_shape;
    VoxelHull                       m_hull;
};