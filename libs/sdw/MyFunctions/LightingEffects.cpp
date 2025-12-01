//
// Created by sajjad on 11/30/25.
//
#include "LightingEffects.h"

#include <algorithm>

uint32_t brightenColour(Colour colour, float brightness, bool isInShadow) {

	brightness = std::min(brightness, 1.0f);
    // brightness = std::max(0.35f, brightness);
    // if (isInShadow) brightness = 0.2f;

	// printf("brightness: %f\n", distanceFromLight * distanceFromLight);
	float litRed = float(colour.red) * brightness;
	float litBlue =  float(colour.blue )* brightness;
	float litGreen =  float(colour.green )* brightness;

	uint32_t updatedColour = (255 << 24) + ((int)litRed << 16) + ((int) litGreen << 8) + (int)litBlue;

	return updatedColour;
}


float updateBrightnessWithAOI(glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, float brightness){

    glm::vec3 vectorToLight = intersection.intersectionPoint - lightSourcePosition;
    vectorToLight = glm::normalize(vectorToLight);
    //Angle of Incident light:
    glm::vec3 normalToSurf = normalize(intersection.intersectedTriangle.normal);

    float angle = glm::dot( vectorToLight, normalToSurf);
    angle = std::max(0.0f, angle);

    brightness *= angle;


    return brightness;
}

float getBrightnessWithProximity(float intensity, glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection) {
    glm::vec3 vectorToLight = intersection.intersectionPoint - lightSourcePosition;

    float distanceFromLight = glm::length(vectorToLight);
    float brightness = intensity * 1.0f / (5.0f * M_PI * std::pow(distanceFromLight, 2) );
    // float brightness = intensity /distanceFromLight;

    //Figure out what to do with that angle to change the brightness of the colours

    return brightness;
}

float brightnessAfterSpecular(glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, glm::vec3 cameraPosition, float brightness) {

    glm::vec3 vectorToLight = intersection.intersectionPoint - lightSourcePosition;
    glm::vec3 normalToSurf = glm::normalize( intersection.intersectedTriangle.normal);

    glm::vec3 ri = glm::normalize(vectorToLight);
    glm::vec3 viewVector = glm::normalize( cameraPosition - intersection.intersectionPoint);

    glm::vec3 reflectionVec = ri - (2.0f * normalToSurf * (glm::dot(ri, normalToSurf )));
    reflectionVec = glm::normalize(reflectionVec);

    float shininess = glm::dot(viewVector, reflectionVec);
    shininess = glm::pow(shininess, 256);
    shininess = std::max(0.0f, shininess);

    brightness = shininess+  brightness ;

    return brightness;
}
uint32_t applyLightingEffects(Colour colour, float intensity, glm::vec3& lightSourcePosition, const RayTriangleIntersection& intersection, glm::vec3 cameraPosition, bool isInShadow) {

    float brightness = getBrightnessWithProximity(intensity, lightSourcePosition, intersection);
    brightness = updateBrightnessWithAOI(lightSourcePosition, intersection, brightness);
    brightness = brightnessAfterSpecular(lightSourcePosition, intersection, cameraPosition, brightness);
    return brightenColour(colour, brightness, isInShadow);

}
