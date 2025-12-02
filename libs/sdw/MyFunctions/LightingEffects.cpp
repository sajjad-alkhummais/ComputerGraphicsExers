//
// Created by sajjad on 11/30/25.
//
#include "LightingEffects.h"

#include <algorithm>

uint32_t brightenColour(Colour colour, float diffuseBrightness, float specularBrightness, bool isInShadow) {


    // brightness = std::max(0.35f, brightness);
    // if (isInShadow) brightness = 0.2f;

	// printf("brightness: %f\n", distanceFromLight * distanceFromLight);
	// float litRed = float(colour.red) * brightness;
	// float litBlue =  float(colour.blue )* brightness;
	// float litGreen =  float(colour.green )* brightness;
    float litRed = float(colour.red) * diffuseBrightness;
    float litBlue =  float(colour.blue )* diffuseBrightness;
    float litGreen =  float(colour.green )* diffuseBrightness;

    litRed += 255.0f * specularBrightness;
    litGreen += 255.0f * specularBrightness;
    litBlue += 255.0f * specularBrightness;
    litRed = std::max(0.0f, std::min(255.0f, litRed));
    litGreen = std::max(0.0f, std::min(255.0f, litGreen));
    litBlue = std::max(0.0f, std::min(255.0f, litBlue));
	uint32_t updatedColour = (255 << 24) + ((int)litRed << 16) + ((int) litGreen << 8) + (int)litBlue;

	return updatedColour;
}


float updateBrightnessWithAOI(glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal, float brightness){

    glm::vec3 vectorToLight = intersectionPoint- lightSourcePosition;
    vectorToLight = glm::normalize(vectorToLight);
    //Angle of Incident light:
    glm::vec3 normalToSurf = normalize(normal);

    float angle = glm::dot( vectorToLight, normalToSurf);
    angle = std::max(0.0f, angle);

    brightness *= angle;


    return brightness;
}

float getBrightnessWithProximity(float intensity, glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal) {
    glm::vec3 vectorToLight = intersectionPoint - lightSourcePosition;

    float distanceFromLight = glm::length(vectorToLight);
    float brightness = intensity * 1.0f / (5.0f * M_PI * std::pow(distanceFromLight, 2) );
    // float brightness = intensity /distanceFromLight;

    //Figure out what to do with that angle to change the brightness of the colours

    return brightness;
}

float brightnessAfterSpecular(glm::vec3& lightSourcePosition,const glm::vec3& intersectionPoint, glm::vec3 normal, glm::vec3 cameraPosition, float brightness) {

    glm::vec3 vectorToLight = (intersectionPoint - lightSourcePosition);
    glm::vec3 normalToSurf = glm::normalize( normal);

    glm::vec3 ri = glm::normalize(vectorToLight);
    glm::vec3 viewVector = glm::normalize( cameraPosition - intersectionPoint);

    glm::vec3 reflectionVec = ri - (2.0f * normalToSurf * (glm::dot(ri, normalToSurf )));
    reflectionVec = glm::normalize(reflectionVec);

    float shininess = glm::dot(viewVector, reflectionVec);
    shininess = std::max(0.0f, shininess);
    shininess = glm::pow(shininess, 32);

    // brightness = shininess+  brightness ;

    return shininess;
    return brightness;
}


uint32_t applyLightingEffects(Colour colour, float intensity, glm::vec3& lightSourcePosition, const glm::vec3& intersectionPoint, glm::vec3 normal, glm::vec3 cameraPosition, bool isInShadow) {
    float diffuseBrightness;

    diffuseBrightness = getBrightnessWithProximity(intensity, lightSourcePosition, intersectionPoint, normal);
    diffuseBrightness = updateBrightnessWithAOI(lightSourcePosition, intersectionPoint, normal, diffuseBrightness);
	diffuseBrightness = std::min(diffuseBrightness, 1.0f);

    float specularBrightness = 0.0f;
    specularBrightness = brightnessAfterSpecular(lightSourcePosition, intersectionPoint, normal, cameraPosition, diffuseBrightness);

    return brightenColour(colour, diffuseBrightness, specularBrightness, isInShadow);

}

uint32_t applyGouraud( float intensity,
    Colour baseColour,
    std::array<glm::vec3, 3> triangleVertices, std::vector<glm::vec3> normal, glm::vec3 cameraPosition,  glm::vec3& lightSourcePosition, bool isInShadow,
	glm::vec3 barycentericValues
) {


    std::vector<glm::vec3> colours;
    for (int i = 0; i<3; i++) {
        // float brightness ;
        float diffuseBrightness;
        float specularBrightness = 0.0f;
        // if (isInShadow) {
        //     brightness = 0.2f;
        //     // brightness = 0;
        // }else {
            diffuseBrightness = getBrightnessWithProximity(intensity, lightSourcePosition, triangleVertices[i], normal[i]);
            diffuseBrightness = updateBrightnessWithAOI(lightSourcePosition, triangleVertices[i], normal[i], diffuseBrightness);
            diffuseBrightness = std::min(diffuseBrightness, 1.0f);
            specularBrightness = brightnessAfterSpecular(lightSourcePosition, triangleVertices[i], normal[i], cameraPosition, diffuseBrightness);
        // }

        float litRed = float(baseColour.red) * diffuseBrightness;
        float litBlue =  float(baseColour.blue )* diffuseBrightness;
        float litGreen =  float(baseColour.green )* diffuseBrightness;

        litRed += 255.0f * specularBrightness;
        litGreen += 255.0f * specularBrightness;
        litBlue += 255.0f * specularBrightness;

        glm::vec3 colourAsVector(litRed, litGreen, litBlue);
        colours.push_back(colourAsVector);
    }

    glm::vec3 finalCol = colours[0] * barycentericValues[0] + colours[1] * barycentericValues[1] + colours[2] * barycentericValues[2];
    finalCol[0] = std::max(0.0f, std::min(255.0f, finalCol[0]));
    finalCol[1] = std::max(0.0f, std::min(255.0f, finalCol[1]));
    finalCol[2] = std::max(0.0f, std::min(255.0f, finalCol[2]));
    uint32_t updatedColour = (255 << 24) + ((int)finalCol[0] << 16) + ((int) finalCol[1] << 8) + (int)finalCol[2];
    // Temporary debug visualization

    return updatedColour;





}