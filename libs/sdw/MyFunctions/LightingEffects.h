//
// Created by sajjad on 11/30/25.
//


#ifndef REDNOISE_LIGHTINGEFFECTS_H
#define REDNOISE_LIGHTINGEFFECTS_H

#include "Shading.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "Texturing.h"
#endif //REDNOISE_LIGHTINGEFFECTS_H

float getBrightnessWithProximity(float intensity, glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal);
float updateBrightnessWithAOI(glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal, float brightness);
float getSpecularFactor(int shininessFactor, glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal, float brightness);
Colour brightenColour(Colour colour, float diffuseBrightness, float specularBrightness, bool isInShadow);
Colour applyLightingEffects(Colour colour, float intensity, glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal, glm::vec3 cameraPosition, bool isInShadow);
Colour applyGouraud( float intensity,
    Colour baseColour,
    std::array<glm::vec3, 3> triangleVertices,
    std::vector<glm::vec3> uniqueVertices,
    std::vector<glm::vec3> vertexNormals, glm::vec3 cameraPosition,  glm::vec3& lightSourcePosition, bool isInShadow,
    glm::vec3 barycentericValues
);