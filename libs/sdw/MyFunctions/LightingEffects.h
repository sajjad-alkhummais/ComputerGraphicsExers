//
// Created by sajjad on 11/30/25.
//


#ifndef REDNOISE_LIGHTINGEFFECTS_H
#define REDNOISE_LIGHTINGEFFECTS_H

#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "Texturing.h"
#endif //REDNOISE_LIGHTINGEFFECTS_H

float getBrightnessWithProximity(float intensity, glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection);
float updateBrightnessWithAOI(glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, float brightness);
float brightnessAfterSpecular(glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, float brightness);
uint32_t brightenColour(Colour colour, float brightness, bool isInShadow);
uint32_t applyLightingEffects(Colour colour, float intensity, glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, glm::vec3 cameraPosition, bool isInShadow);
