//
// Created by sajjad on 11/7/25.
//


#include "Raytracing.h"

#include "Shading.h"
#include "glm/gtx/string_cast.hpp"

RayTriangleIntersection getClosestValidIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray) {

	RayTriangleIntersection intersection;
	float closestIntersection = INFINITY;

	// int triangleIndex = 0;
	intersection.intersectionFound = false;
	for (ModelTriangle triangle : theTriModels) {
		glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
		glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		glm::vec3 SPVector = cameraPosition - triangle.vertices[0];
		glm::mat3 DEMatrix(-rayDirection, e0, e1);
		glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
		float distanceFromCamera = possibleSolution.x;
		if (distanceFromCamera < 1e-4f)
			continue; // ignore self-hits

		float u, v;
		u = possibleSolution.y;
		v = possibleSolution.z;
		//std::cout << possibleSolution.x << std::endl;
		bool validIntersection = (u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0 && distanceFromCamera >= 0;
		//&& t > focalLength;
		if (validIntersection) {
			//printf("in\n");
			intersection.intersectionFound = true;
			if (distanceFromCamera < closestIntersection) {
				glm::vec3 interpolationValues(1 - u - v, u, v);
				intersection.barycentericValues = interpolationValues;

				closestIntersection = distanceFromCamera;
				intersection.intersectedTriangle = triangle;
				intersection.distanceFromCamera = distanceFromCamera;
				intersection.triangleIndex = triangle.triangleIndex;
				intersection.intersectionPoint =  cameraPosition +  distanceFromCamera * rayDirection; //This is relative to the world (converted by shifting by camera poisiton)



				//Check if the intersection is with a textured triangle
				if (intersection.intersectedTriangle.hasTexture){

					intersection.textureColourAsInt = getTextureColourOfIntersection(textureArray, triangle, u, v);

				}





			}
		}

			// triangleIndex++;
	}

	return intersection;
}

// void testGetClosestValidIntersection(std::vector<ModelTriangle> theTriModels ) {
// 	glm::vec3 rayDirection(0,0,-4.0);
// 	glm::vec3 cameraPosition = glm::vec3(0.0, 0.0,4.0);
//
	// RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection, theTriModels);
	// std::cout << intersection << std::endl;
//
//
// }
void renderRaytracedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels , std::vector<std::vector<uint32_t>> &textureArray, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength) {
	window.clearPixels();
	float imagePlaneScaling = 1.0/160;
	for (int y = 0; y< HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {


			float x3D = (x - WIDTH / 2.0f) * imagePlaneScaling;
			float y3D = (y - HEIGHT / 2.0f) * imagePlaneScaling;
			// float imagePlaneZValue = cameraPosition.z - focalLength; //World coords
			glm::vec3 localRayDirection( x3D, -y3D, focalLength);

			// pixelToTraceIn3D = pixelToTraceIn3D * cameraOrientation;
			glm::vec3 rayDirection;
			rayDirection = cameraOrientation * localRayDirection;
			rayDirection = glm::normalize(rayDirection) ;
			RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection, theTriModels, textureArray );

			if (intersection.intersectionFound) {


				//printf("inside %d, %d \n", x, y);
				uint32_t colour = convertColourToInt(
					Colour(intersection.intersectedTriangle.colour));

				window.setPixelColour(x, y, colour);
			}

		}
	}



}


bool isInShadow(std::vector<ModelTriangle> &theTriModels,  std::vector<std::vector<uint32_t>> &textureArray, glm::vec3 sourcePosition, glm::vec3 lightSourcePosition) {

	glm::vec3 intersectionPointOfSurface = sourcePosition; //in 3d
	glm::vec3 shadowRayDirection = glm::normalize(lightSourcePosition - intersectionPointOfSurface); //From surface to light source
	RayTriangleIntersection shadowRayIntersection = getClosestValidIntersection(intersectionPointOfSurface, shadowRayDirection, theTriModels, textureArray);
	float distanceFromSurfaceToIntersection = shadowRayIntersection.distanceFromCamera;
	float distanceFromSurfaceToLightSource = glm::distance(lightSourcePosition, intersectionPointOfSurface);
	bool inShadow = shadowRayIntersection.intersectionFound
	&& distanceFromSurfaceToIntersection < distanceFromSurfaceToLightSource; //Condition implies that the shadow ray has collided
//	with triangles before reaching the light source, so this point is in shadow.

	return inShadow;

}


RayTriangleIntersection getReflectedRay(const RayTriangleIntersection& initialIntersection, glm::vec3 incidentRay, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray) {
	glm::vec3 normalToSurf = glm::normalize(initialIntersection.intersectedTriangle.normal);
	glm::vec3 reflectionVec = incidentRay - (2.0f * normalToSurf * (glm::dot(incidentRay, normalToSurf )));
	reflectionVec = glm::normalize(reflectionVec);
	RayTriangleIntersection reflectionIntersection = getClosestValidIntersection(initialIntersection.intersectionPoint, reflectionVec, theTriModels, textureArray);
	return reflectionIntersection;
}

void renderRaytracedModelWithShadows(DrawingWindow &window,
	std::vector<ModelTriangle> &theTriModels,
	std::vector<std::vector<uint32_t>> &textureArray,
	glm::vec3 cameraPosition,
	glm::mat3 cameraOrientation,
	std::vector<glm::vec3> lightSourcePositions,
	float focalLength,
	std::vector<glm::vec3> uniqueVertices,
	std::vector<glm::vec3> vertexNormals,
	int shadingType){
	float imagePlaneScaling = 1.0/160;

	#pragma omp parallel for

	for (int y = 0; y< HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {


			//raytracer's image plane move with the camera's X and Y position, so camera position is kind of the center

			float x3D = (x - WIDTH / 2.0f) * imagePlaneScaling ;
			float y3D = (y - HEIGHT / 2.0f) * imagePlaneScaling;
			glm::vec3 localRayDirection( x3D, -y3D, focalLength);

			glm::vec3 rayDirection;
			rayDirection = cameraOrientation * localRayDirection ;
			rayDirection = glm::normalize(rayDirection);
			RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection, theTriModels, textureArray);
			if (intersection.intersectionFound) {

				// bool modelIsTheSphere = theTriModels.size() > 45;
				bool modelIsTheSphere = intersection.triangleIndex > 31;


				// Colour colour = intersection.intersectedTriangle.colour;
				Colour colour;

				//10 and 11 is right wall
				//31 and 26 is blue box front
				bool mirroredSurface = intersection.intersectedTriangle.triangleIndex == 10 || intersection.intersectedTriangle.triangleIndex == 11;
				glm::vec3 viewingPosition = cameraPosition;
				if (mirroredSurface && !modelIsTheSphere) {
					RayTriangleIntersection reflectionIntersection = getReflectedRay(intersection, rayDirection, theTriModels, textureArray);

					// cameraPosition = intersection.intersectionPoint;
					intersection = reflectionIntersection;
					//Update color and all relevent variables for lighting effects to reflect correctly.
					// colour = reflectionIntersection.intersectedTriangle.colour;
					// intersectionPointOfSurface = reflectionIntersection.intersectionPoint;
					// intersection.intersectedTriangle.normal = reflectionIntersection.intersectedTriangle.normal;
					viewingPosition = reflectionIntersection.intersectionPoint;
					modelIsTheSphere = reflectionIntersection.triangleIndex > 31;
				}

				glm::vec3 intersectionPointOfSurface = intersection.intersectionPoint; //in 3d
				colour = intersection.intersectedTriangle.colour;
				if (intersection.intersectedTriangle.hasTexture) {

					colour.red   = (intersection.textureColourAsInt >> 16) & 0xff;
					colour.green = (intersection.textureColourAsInt >> 8)  & 0xff;
					colour.blue  =  intersection.textureColourAsInt        & 0xff;

				}

				//interpolateNormals using phong

				uint32_t finalColour = 0;
				Colour brightenedColour;
				glm::vec3 brightenedColours;

				int shadowCollisions = 0;
				for (glm::vec3 lightSourcePosition : lightSourcePositions) {
					bool inShadow = isInShadow(theTriModels,textureArray, intersectionPointOfSurface, lightSourcePosition);
					// float shadowPercentage = isInSoftShadow(theTriModels,textureArray, intersectionPointOfSurface, lightSourcePositions);
					if (inShadow) shadowCollisions++;
					else {

					}

					//Apply Lighting or shading
					float intensity = 35;

					if (!modelIsTheSphere) {
						brightenedColour = applyLightingEffects(colour, intensity,  lightSourcePosition, intersectionPointOfSurface, intersection.intersectedTriangle.normal, viewingPosition, inShadow);
					}
					else {
						if (shadingType == 1) {
							glm::vec3 interpolatedNormal = getNormalUsingPhong(intersection, uniqueVertices, vertexNormals);
							brightenedColour = applyLightingEffects(colour, intensity,  lightSourcePosition, intersectionPointOfSurface, interpolatedNormal,
								viewingPosition, inShadow);

						} else {
							brightenedColour = applyGouraud(intensity, colour, intersection.intersectedTriangle.vertices,
							uniqueVertices, vertexNormals, cameraPosition,
							lightSourcePosition, inShadow, intersection.barycentericValues );
						}


					}
					glm::vec3 colourAsVector(brightenedColour.red, brightenedColour.green, brightenedColour.blue);
					brightenedColours += colourAsVector;


				}
				//Averaging each colour channel:

				brightenedColours = brightenedColours/ static_cast<float>(lightSourcePositions.size());
				brightenedColour.red = brightenedColours.x;
				brightenedColour.green = brightenedColours.y;
				brightenedColour.blue = brightenedColours.z;
				finalColour = convertColourToInt(brightenedColour);
				//Check texturing: (lighting and mirroring on textures not implemented)



				window.setPixelColour(x, y, finalColour);
			}


		}
	}



}
