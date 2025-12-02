//
// Created by sajjad on 12/1/25.
//


#include "Shading.h"

glm::vec3 getVertexNormal(glm::vec3 vertex,
                          std::vector<glm::vec3>& uniqueVertices,
                          std::vector<glm::vec3>& vertexNormals) {

    for (int i = 0; i < uniqueVertices.size(); i++) {
        if (uniqueVertices[i] == vertex) {
            return vertexNormals[i];
        }
    }
    return glm::vec3(0, 1, 0); // default if not founddef
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