//

// Created by sajjad on 11/8/25.
//
#include "LoadingFiles.h"
std::vector<Colour> loadColours() {
	std::string s;
	std::vector<Colour> colours;
	std::ifstream f("cornell-box.mtl");

	if (!f.is_open())
		std::cerr << "error" ;

	std::string colourName;
	while (getline(f, s)) {

		if (s.empty() || s[0] == '#') continue;


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
		//Dealing with colours

	}


	return colours;
}
std::vector<ModelTriangle> loadModel(float scaling) {
	std::string s;
	std::vector<ModelTriangle> triModels;
	std::ifstream f("cornell-box.obj");

	if (!f.is_open())
		std::cerr << "error" ;

	Colour colourObj;
	std::vector<glm::vec3> vertices;
	while (getline(f, s)) {


		if (s.empty()) continue;


			if (s.at(0) == 'v') {
				std::vector<std::string> points = split(s, ' ');
				glm::vec3 vectorN = glm::vec3(std::stof(points[1]),
					std::stof(points[2]),
					std::stof(points[3]));
				vertices.push_back(vectorN);
			}
			else if (s.at(0) == 'f') {
				std::vector<std::string> indexes = split(s, ' ');

				auto pos1 = indexes[1].find('/');
				auto pos2 = indexes[2].find('/');
				auto pos3 = indexes[3].find('/');
				indexes[1] = (indexes[1].substr(0, pos1));
				indexes[2] = (indexes[2].substr(0, pos2));
				indexes[3] = ( indexes[3].substr(0, pos3));
				// the vertiecs of the specifed index is in the triangle

				 ModelTriangle tri = ModelTriangle(vertices[std::stoi(indexes[1]) - 1] * scaling,
					vertices[std::stoi(indexes[2]) - 1] * scaling,
					vertices[std::stoi(indexes[3]) - 1] * scaling, colourObj);
				triModels.push_back(tri);
			}
		//Dealing with colours

			else if (s.at(0) == 'u') {
				std::string colourName = split(s, ' ')[1];
				std::vector<Colour> coloursPalette = loadColours();

				for (Colour colour_i : coloursPalette) {

					if ( colour_i.name == colourName) {
						colourObj = colour_i;
					}
				}

			}
	}

	return triModels;
}
void test_loadModel() {

	std::vector<ModelTriangle> theTriModels = loadModel(1);


	for (ModelTriangle singleTri : theTriModels) {
		std::cout<< singleTri << std::endl;
	}
}