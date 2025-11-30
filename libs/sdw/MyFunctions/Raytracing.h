//
// Created by sajjad on 11/7/25.
//

#ifndef REDNOISE_RAYTRACING_H
#define REDNOISE_RAYTRACING_H
#include "CanvasPoint.h"
#include <glm/glm.hpp>

#include "DrawingWindow.h"
#endif //REDNOISE_RAYTRACING_H
#include "ModelTriangle.h"
#include "vector"
#include "LoadingFiles.h"
#include <glm/glm.hpp>

#include "RayTriangleIntersection.h"
RayTriangleIntersection getClosestValidIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray);
void testGetClosestValidIntersection(std::vector<ModelTriangle> theTriModels );
void renderRaytracedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength);
void renderRaytracedModelWithShadows(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray, glm::vec3 cameraPosition, glm::mat3 cameraOrientation, glm::vec3 lightSourcePosition, float focalLength);
bool isInShadow(std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray, glm::vec3 sourcePosition, glm::vec3 lightSourcePosition);
