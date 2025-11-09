//
// Created by sajjad on 11/8/25.
//

#ifndef REDNOISE_LOADINGFILES_H
#define REDNOISE_LOADINGFILES_H

#endif //REDNOISE_LOADINGFILES_H

#include "vector"
#include "string"
#include "Colour.h"
#include <fstream>
#include "ModelTriangle.h"
#include "Utils.h"

std::vector<Colour> loadColours();

std::vector<ModelTriangle> loadModel(float scaling);
void test_loadModel();
