//
// Created by sajjad on 11/30/25.
//

#ifndef REDNOISE_WIREFRAME_H
#define REDNOISE_WIREFRAME_H

#endif //REDNOISE_WIREFRAME_H
#define WIDTH 320
#define HEIGHT 240

#include "CanvasPoint.h"
#include "MyFunctions/Projection.h"
#include "Colour.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "CanvasTriangle.h"
#include "Utils.h"
#include "glm/glm.hpp"
void drawingALine(DrawingWindow &window, CanvasPoint start, CanvasPoint end, Colour clr);
void testDrawingALine(DrawingWindow &window);
std::vector<float> interpolateSingleFloats(float from, float to, int numOfValues);


void test_interpolateSingleFloats();
void drawingGreyScale(DrawingWindow& window);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numOfValues);
void test_interpolateThreeElementValues();
void drawing2DColourInterpolation(DrawingWindow &window);
void drawingTriangleRGB(DrawingWindow &window);
void draw(DrawingWindow &window);

void drawingATriangle(DrawingWindow &window, CanvasTriangle triangle, Colour clr);

void fillingHalfTriangle(DrawingWindow &window, CanvasTriangle tri, Colour clr);

void drawingFilledTriangles(DrawingWindow &window, CanvasTriangle triangle, Colour clr);

void testFillingATriangle(DrawingWindow &window);
void testDrawingATriangle(DrawingWindow &window);

void renderSketchedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength );
