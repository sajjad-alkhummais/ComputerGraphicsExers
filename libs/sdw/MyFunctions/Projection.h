//
// Created by sajjad on 11/30/25.
//

#ifndef REDNOISE_PROJECTION_H
#define REDNOISE_PROJECTION_H

#endif //REDNOISE_PROJECTION_H
#include "glm/glm.hpp"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "LoadingFiles.h"
#define WIDTH 320
#define HEIGHT 240


CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition);

// Overloaded projection function for an oriented camera (position and orientation matrix)
CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, glm::vec3 vertexPosition);

// Function to render all points in a model (used for basic visual debugging)
void renderClouds(DrawingWindow &window);

// Overloaded function to convert a 3D triangle to a 2D canvas triangle (fixed camera)
CanvasTriangle convert3DTriTo2D(ModelTriangle triangleIn3D, glm::vec3 cameraPosition, float focalLength);

// Overloaded function to convert a 3D triangle to a 2D canvas triangle (oriented camera)
CanvasTriangle convert3DTriTo2D(ModelTriangle triangleIn3D, glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength);
