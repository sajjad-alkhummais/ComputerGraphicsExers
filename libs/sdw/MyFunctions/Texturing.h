//
// Created by sajjad on 11/30/25.
//

#ifndef REDNOISE_TEXTURING_H
#define REDNOISE_TEXTURING_H
#include "ModelTriangle.h"

#endif //REDNOISE_TEXTURING_H


#include "CanvasTriangle.h"
#include "Colour.h"
#include "DrawingWindow.h"
#include "TextureMap.h"
#include "Wireframe.h"
void textureHalfTriangle(DrawingWindow &window, CanvasTriangle theTri, CanvasTriangle textureTri, std::vector<std::vector<uint32_t>> textureArray);
std::vector<std::vector<uint32_t>> createTextureArray(TextureMap &textureFile);
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle theTri, TextureMap &textureFile);
void testATexturedTri(DrawingWindow &window);

uint32_t getTextureColourOfIntersection(std::vector<std::vector<uint32_t>> &textureArray, ModelTriangle intersectedTriangle, float u, float v);