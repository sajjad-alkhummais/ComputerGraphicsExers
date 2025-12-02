//
// Created by sajjad on 11/7/25.
//


#include "Raytracing.h"

#include "Shading.h"
#include "glm/gtx/string_cast.hpp"

#define WIDTH 320
#define HEIGHT 240
RayTriangleIntersection getClosestValidIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> &textureArray) {

	RayTriangleIntersection intersection;
	float closestIntersection = INFINITY;

	int triangleIndex = 0;
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
				intersection.triangleIndex = triangleIndex;
				intersection.intersectionPoint =  cameraPosition +  distanceFromCamera * rayDirection; //This is relative to the world (converted by shifting by camera poisiton)



				//Check if the intersection is with a textured triangle
				if (intersection.intersectedTriangle.hasTexture){

					intersection.textureColourAsInt = getTextureColourOfIntersection(textureArray, triangle, u, v);

				}





			}
		}

			triangleIndex++;
	}
	//To recoginize when there isn't any valid intersections

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





void renderRaytracedModelWithShadows(DrawingWindow &window,
	std::vector<ModelTriangle> &theTriModels,
	std::vector<std::vector<uint32_t>> &textureArray,
	glm::vec3 cameraPosition,
	glm::mat3 cameraOrientation,
	glm::vec3 lightSourcePosition,
	float focalLength,
	std::vector<glm::vec3> uniqueVertices,
	std::vector<glm::vec3> vertexNormals){
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

				//printf("inside %d, %d \n", x, y);

				glm::vec3 intersectionPointOfSurface = intersection.intersectionPoint; //in 3d

				//Goraud
				glm::vec3 v0 = intersection.intersectedTriangle.vertices[0];
				glm::vec3 v1 = intersection.intersectedTriangle.vertices[1];
				glm::vec3 v2 = intersection.intersectedTriangle.vertices[2];

				glm::vec3 normal0 = getVertexNormal(v0, uniqueVertices, vertexNormals);
				glm::vec3 normal1 = getVertexNormal(v1, uniqueVertices, vertexNormals);

				glm::vec3 normal2 = getVertexNormal(v2, uniqueVertices, vertexNormals);

				std::vector<glm::vec3> normals;
				normals.push_back(normal0);
				normals.push_back(normal1);
				normals.push_back(normal2);
				float u = intersection.barycentericValues.x;
				float v = intersection.barycentericValues.y;
				float w = intersection.barycentericValues.z;
				glm::vec3 interpolatedNormal = glm::normalize(u * normal0 + v * normal1 + w * normal2);


				bool isLit = !isInShadow(theTriModels,textureArray, intersectionPointOfSurface, lightSourcePosition);
				Colour colour = intersection.intersectedTriangle.colour;
				uint32_t finalColour = 0;

				// finalColour = applyLightingEffects(colour, 40,  lightSourcePosition, intersectionPointOfSurface, intersection.intersectedTriangle.normal, cameraPosition, !isLit);
				finalColour = applyLightingEffects(colour, 30,  lightSourcePosition, intersectionPointOfSurface, interpolatedNormal, cameraPosition, !isLit);
				// finalColour = applyGouraud(25, colour, intersection.intersectedTriangle.vertices, normals, cameraPosition, lightSourcePosition, !isLit, intersection.barycentericValues );


				//Check texturing:
				if (intersection.intersectedTriangle.hasTexture)
					finalColour = intersection.textureColourAsInt;


				window.setPixelColour(x, y, finalColour);
			}


		}
	}



}
