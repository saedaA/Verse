#pragma once

#include <glm/glm.hpp>

struct Voxel
{
    Voxel(const glm::uvec3 & cell):
        cell(cell)
    {
    }

    Voxel(const glm::uvec3 & cell, const glm::vec3 & color, float healthPoints):
        cell(cell),
        color(color),
        healthPoints(healthPoints)
    {}

    glm::uvec3  cell;
    glm::vec3   color;
    float       healthPoints;
};