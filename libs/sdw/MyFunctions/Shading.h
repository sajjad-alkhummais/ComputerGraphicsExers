//
// Created by sajjad on 12/1/25.
//

#ifndef REDNOISE_SHADING_H
#define REDNOISE_SHADING_H

#include "glm/glm.hpp"
#include <vector>

#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"
#endif //REDNOISE_SHADING_H
std::vector<glm::vec3> calculateVertexNormals(std::vector<ModelTriangle> triangles,    std::vector<glm::vec3> uniqueVertices);

std::vector<glm::vec3> getUniqueVertices(std::vector<ModelTriangle> triangles) ;
//Gets the normal corresponding to the given vertex
glm::vec3 getVertexNormal(glm::vec3 vertex,
    std::vector<glm::vec3>& uniqueVertices,
    std::vector<glm::vec3>& vertexNormals) ;

glm::vec3 getNormalUsingPhong(RayTriangleIntersection intersection,  std::vector<glm::vec3> uniqueVertices,  std::vector<glm::vec3> vertexNormals);
