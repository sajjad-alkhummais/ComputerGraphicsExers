//
// Created by sajjad on 12/1/25.
//


#include "Shading.h"

glm::vec3 getVertexNormal(glm::vec3 vertex,
                          std::vector<glm::vec3>& uniqueVertices,
                          std::vector<glm::vec3>& vertexNormals) {

    for (int i = 0; i < uniqueVertices.size(); i++) {
        float distance = glm::length(uniqueVertices[i] - vertex);
        if (distance < 0.0001f) {  // Vertices within 0.0001 units are "equal"
            return vertexNormals[i];
        }
    }
    printf("problem\n");
    return glm::vec3(0, 1, 0); // default
}
std::vector<glm::vec3> getUniqueVertices(std::vector<ModelTriangle> triangles) {
    std::vector<glm::vec3> uniqueVertices;

    for (ModelTriangle triangle : triangles) {
        for (glm::vec3 vertex : triangle.vertices) {
            bool added = false;
            for (glm::vec3 foundVertex : uniqueVertices) {
                if (foundVertex == vertex)
                    added = true;
                break;
            }
            if (!added) {
                uniqueVertices.push_back(vertex);
            }

        }
    }
    return uniqueVertices;
}
std::vector<glm::vec3> calculateVertexNormals(std::vector<ModelTriangle> triangles,    std::vector<glm::vec3> uniqueVertices) {

    std::vector<glm::vec3> vertexNormals;
    for (glm::vec3 uniqueVertex : uniqueVertices) {
        std::vector<glm::vec3> adjacentTrianglesNormals;
        for (ModelTriangle triangle : triangles) {
            for (glm::vec3 vi : triangle.vertices) {
                if (uniqueVertex == vi) {
                    adjacentTrianglesNormals.push_back(triangle.normal);
                    break;
                }
            }


        }
        glm::vec3 averageNormal(0,0,0);
        for (glm::vec3 normal : adjacentTrianglesNormals) {
            averageNormal += normal;
        }

        float numOfAdjacents = adjacentTrianglesNormals.size();
        averageNormal = averageNormal/numOfAdjacents;

        vertexNormals.push_back(glm::normalize(averageNormal));
    }

    return vertexNormals;
}

glm::vec3 getNormalUsingPhong(RayTriangleIntersection intersection,  std::vector<glm::vec3> uniqueVertices,  std::vector<glm::vec3> vertexNormals) {


    glm::vec3 v0 = intersection.intersectedTriangle.vertices[0];
    glm::vec3 v1 = intersection.intersectedTriangle.vertices[1];
    glm::vec3 v2 = intersection.intersectedTriangle.vertices[2];

    glm::vec3 normal0 = getVertexNormal(v0, uniqueVertices, vertexNormals);
    glm::vec3 normal1 = getVertexNormal(v1, uniqueVertices, vertexNormals);
    glm::vec3 normal2 = getVertexNormal(v2, uniqueVertices, vertexNormals);

    std::vector<glm::vec3> normals;
    normals.push_back(normal0);
    normals.push_back(normal1);
    normals.push_back(normal2);
    float u = intersection.barycentericValues.x;
    float v = intersection.barycentericValues.y;
    float w = intersection.barycentericValues.z;
    glm::vec3 interpolatedNormal = glm::normalize(u * normal0 + v * normal1 + w * normal2);
    return interpolatedNormal;
}
