//
// Created by sajjad on 11/7/25.
//


#include "Raytracing.h"

#include "DrawingWindow.h"
#define WIDTH 320
#define HEIGHT 240
RayTriangleIntersection getClosestValidIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection) {

	float scaling = 0.35;
	RayTriangleIntersection intersection;
	float closestIntersection = INFINITY;

	int triangleIndex = 0;
	bool foundValidIntersection = false;
	std::vector<ModelTriangle> theTriModels = loadModel(scaling);
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
			foundValidIntersection = true;
			if (distanceFromCamera < closestIntersection) {
				closestIntersection = distanceFromCamera;
				intersection.intersectedTriangle = triangle;
				intersection.distanceFromCamera = distanceFromCamera;
				intersection.triangleIndex = triangleIndex;
				intersection.intersectionPoint =  cameraPosition +  distanceFromCamera * rayDirection; //This is relative to the world (converted by shifting by camera poisiton)
			//	intersection.intersectionPoint = triangle.vertices[0] + possibleSolution.y * (triangle.vertices[1]  - triangle.vertices[0]) // This is relative to world origin
			//	+ possibleSolution.z * (triangle.vertices[2]  - triangle.vertices[0]);
			}
		}
		triangleIndex++;
	}

	//To recoginize when there isn't any valid intersections
	if (foundValidIntersection == false) intersection.triangleIndex = -1;

	return intersection;
}

void testGetClosestValidIntersection() {
	glm::vec3 rayDirection(0,0,-4.0);
	glm::vec3 cameraPosition = glm::vec3(0.0, 0.0,4.0);

	RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection);
	std::cout << intersection << std::endl;


}
void renderRaytracedModel(DrawingWindow &window,glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength , float scaling) {
	scaling = 1.0/160;
	float imagePlaneZValue = cameraPosition.z - focalLength; //World coords
	for (int y = 0; y< HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {


			//printf("outside %d, %d \n", x, y);
			//glm::vec3 pixelToTraceIn3D = glm::vec3(x, y, imagePlaneZValue);
			float ndcX = (x - WIDTH / 2.0f) * scaling;
			float ndcY = (y - HEIGHT / 2.0f) * scaling;
			glm::vec3 pixelToTraceIn3D(ndcX, -ndcY, imagePlaneZValue);


			glm::vec3 rayDirection;
			rayDirection = pixelToTraceIn3D - cameraPosition;
			rayDirection = glm::normalize(rayDirection);
			RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection);

			if (intersection.triangleIndex != -1) {

				//printf("inside %d, %d \n", x, y);
				uint32_t colour = convertColourToInt(
					Colour(intersection.intersectedTriangle.colour));
				window.setPixelColour(x, y, colour);
			}

		}
	}



}

bool isInShadow(glm::vec3 sourcePosition, glm::vec3 lightSourcePosition) {

	glm::vec3 intersectionPointOfSurface = sourcePosition; //in 3d
	glm::vec3 shadowRayDirection = glm::normalize(lightSourcePosition - intersectionPointOfSurface); //From surface to light source
	RayTriangleIntersection shadowRayIntersection = getClosestValidIntersection( intersectionPointOfSurface, shadowRayDirection);
	float distanceFromSurfaceToIntersection = shadowRayIntersection.distanceFromCamera;
	float distanceFromSurfaceToLightSource = glm::distance(lightSourcePosition, intersectionPointOfSurface);
	bool inShadow = shadowRayIntersection.triangleIndex != -1
	&& distanceFromSurfaceToIntersection < distanceFromSurfaceToLightSource; //Condition implies that the shadow ray has collided
//	with triangles before reaching the light source, so this point is in shadow.

	return inShadow;

}
void renderRaytracedModelWithShadows(DrawingWindow &window, glm::vec3 cameraPosition, glm::vec3 lightSourcePosition, float focalLength, SDL_Event &event){
	float imagePlaneScaling = 1.0/160;

	for (int y = 0; y< HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {


			window.pollForInputEvents(event);

			//raytracer's image plane move with the camera's X and Y position, so camera position is kind of the center

			float x3D = (x - WIDTH / 2.0f) * imagePlaneScaling + cameraPosition.x;
			float y3D = cameraPosition.y -  (y - HEIGHT / 2.0f) * imagePlaneScaling;
			float imagePlaneZValue = cameraPosition.z - focalLength; //World coords
			glm::vec3 pixelToTraceIn3D( x3D, y3D, imagePlaneZValue);

			glm::vec3 rayDirection;
			rayDirection = pixelToTraceIn3D - cameraPosition;
			rayDirection = glm::normalize(rayDirection);
			RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection);
			if (intersection.triangleIndex != -1) {

				//printf("inside %d, %d \n", x, y);
				glm::vec3 intersectionPointOfSurface = intersection.intersectionPoint; //in 3d
				bool inShadow = !isInShadow(intersectionPointOfSurface, lightSourcePosition);
				uint32_t colour = convertColourToInt(
				Colour(intersection.intersectedTriangle.colour));

				if (inShadow)
				window.setPixelColour(x, y, colour);
			}


		}
	}



}
