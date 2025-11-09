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
RayTriangleIntersection getClosestValidIntersection();
void testGetClosestValidIntersection();
void renderRaytracedModel(DrawingWindow &window, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength , float scaling);
void renderRaytracedModelWithShadows(DrawingWindow &window, glm::vec3 cameraPosition, glm::vec3 lightSourcePosition, float focalLength, SDL_Event &event);
bool isInShadow(glm::vec3 sourcePosition, glm::vec3 lightSourcePosition);
