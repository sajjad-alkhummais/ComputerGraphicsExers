//
// Created by sajjad on 11/30/25.
//

#ifndef REDNOISE_CAMERAMOVEMENTS_H
#define REDNOISE_CAMERAMOVEMENTS_H

#endif //REDNOISE_CAMERAMOVEMENTS_H


#include "glm/glm.hpp"
#include "DrawingWindow.h"
#include <unistd.h>
void rotateAroundX(glm::vec3 &cameraPosition, float degrees);

void rotateAroundY(glm::vec3 &cameraPosition, float degrees);

void tilt(glm::mat3 &cameraOrientation, float degrees);

void pan(glm::mat3 &cameraOrientation, float degrees);

void lookAt(glm::vec3 center, glm::mat3 &cameraOrientation, glm::vec3 cameraPosition);
void orbit(DrawingWindow &drawing_window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, bool &orbitStatus);

