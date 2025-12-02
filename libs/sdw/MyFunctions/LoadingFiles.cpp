//

// Created by sajjad on 11/8/25.
//
#include "LoadingFiles.h"

#include "TextureMap.h"

std::vector<Colour> loadColours(std::string nameOfFile) {
	std::string s;
	std::vector<Colour> colours;
	std::ifstream f(nameOfFile);

	if (!f.is_open())
		std::cerr << "error" ;

	std::string colourName;
	while (getline(f, s)) {

		if (s.empty() || s[0] == '#') continue;

		if (!s.empty() && s.back() == '\r') s.pop_back();
		if (s.at(0) == 'n') {

			colourName = split(s, ' ')[1];

		}
		else if (s.at(0) == 'K') {

			int clrValue1 = int(255 * std::stof(split(s, ' ')[1]));
			int clrValue2 = int(255 * std::stof(split(s, ' ')[2]));
			int clrValue3 = int(255 * std::stof(split(s, ' ')[3]));
			Colour theClr = Colour(colourName,
				clrValue1,
				clrValue2,
				clrValue3 );

			colours.push_back(theClr);
		}

	}


	return colours;
}
std::vector<ModelTriangle> loadModel(float scaling, std::string nameOfObjectFile, std::string nameOfMaterialFile) {
	std::string s;
	std::vector<ModelTriangle> triModels;
	std::ifstream f("ModelsFiles/" + nameOfObjectFile);

	TextureMap textureFile = TextureMap("ModelsFiles/texture.ppm");

	int triangleIndex = 0;
	std::vector<Colour> coloursPalette = loadColours("ModelsFiles/" + nameOfMaterialFile);

	if (!f.is_open())
		std::cerr << "error" ;

	Colour colourObj;
	std::vector<glm::vec3> vertices;
	std::vector<TexturePoint> textureRatios;
	while (getline(f, s)) {


		if (s.empty()) continue;
		if (!s.empty() && s.back() == '\r') s.pop_back();

		if (s.size() > 1 && s.at(0) == 'v' && s.at(1) == 't') {
			std::vector<std::string> ratios = split(s, ' ');
			float x = stof(ratios[1]) * textureFile.width;
			float y = stof(ratios[2]) * textureFile.height;


			TexturePoint point = TexturePoint(x, y);
			textureRatios.push_back(point);

		}
		else if (s.at(0) == 'v') {
				std::vector<std::string> points = split(s, ' ');
				glm::vec3 vectorN = glm::vec3(std::stof(points[1]),
					std::stof(points[2]),
					std::stof(points[3]));
				vertices.push_back(vectorN);
			}

		else if (s.at(0) == 'f') {

				std::vector<std::string> indexes = split(s, ' ');
				std::vector<std::string> textureIndex = split(s, ' ');

				auto pos1 = indexes[1].find('/');
				auto pos2 = indexes[2].find('/');
				auto pos3 = indexes[3].find('/');

				std::string strV1 = (indexes[1].substr(0, pos1));
				std::string strV2 = (indexes[2].substr(0, pos2));
				std::string strV3 = (indexes[3].substr(0, pos3));
				// the vertiecs of the specifed index is in the triangle

				 ModelTriangle tri = ModelTriangle(
				 	vertices[std::stoi(strV1) - 1] * scaling,
					vertices[std::stoi(strV2) - 1] * scaling,
					vertices[std::stoi(strV3) - 1] * scaling, colourObj);

				tri.normal = -glm::cross(tri.vertices[1] - tri.vertices[0], tri.vertices[2] - tri.vertices[0]);
				tri.hasTexture = false;
				tri.triangleIndex = triangleIndex;
				triangleIndex++;

				// Storing the texture ratios if they exist:
				bool isTexture = (indexes[1][indexes[1].size() - 1] != '/');

				if (isTexture) {
					tri.hasTexture = true;
					//Get the corresponding texture points vertices
					indexes = split(s, ' ');
					int texturePointIndex1 = std::stoi(indexes[1].substr(pos1 + 1));
					int texturePointIndex2 = std::stoi(indexes[2].substr(pos2 + 1));
					int texturePointIndex3 = std::stoi(indexes[3].substr(pos3 + 1));

					TexturePoint texturePoint1 = textureRatios[texturePointIndex1 - 1];
					TexturePoint texturePoint2 = textureRatios[texturePointIndex2 - 1];
					TexturePoint texturePoint3 = textureRatios[texturePointIndex3 - 1];

					std::array<TexturePoint, 3> texture_points = {texturePoint1, texturePoint2, texturePoint3};
					tri.texturePoints = texture_points;

				}
				triModels.push_back(tri);
			}
		//Dealing with colours

		else if (s.at(0) == 'u') {
				std::string colourName = split(s, ' ')[1];

				for (Colour colour_i : coloursPalette) {

					if ( colour_i.name == colourName) {
						colourObj = colour_i;
					}
				}

			}
	}

	return triModels;
}
void test_loadUntexturedModel() {

	std::vector<ModelTriangle> theTriModels = loadModel(1,"cornell-box.obj", "cornell-box.mtl" );


	for (ModelTriangle singleTri : theTriModels) {
		std::cout<< singleTri << std::endl;
	}
}
void test_loadTexturedModel() {
	std::vector<ModelTriangle> theTriModels = loadModel(1,"textured-cornell-box.obj", "cornell-box.mtl" );


	for (ModelTriangle singleTri : theTriModels) {
		std::cout<< singleTri << std::endl;
	}
}
