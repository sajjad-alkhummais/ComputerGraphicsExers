//
// Created by sajjad on 11/30/25.
//
#include "Projection.h"

#include "CanvasTriangle.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"

#include "LoadingFiles.h"
CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition) {

	float u;
	float v;
	float planeScaler = 160;
	//Review x and y, they maybe incorrect, even the z
	// float x = (vertexPosition.x - cameraPosition.x);
	// float y = ( vertexPosition.y - cameraPosition.y);
	// float z = (cameraPosition.z - vertexPosition.z);
	// if (z < 1e-6f) z = 1e-6f;
	glm::vec3 cameraToVertex = vertexPosition - cameraPosition;
	cameraToVertex.z = -cameraToVertex.z; //I have no idea why this is needed for this version of this function

	u = focalLength * ( cameraToVertex.x * planeScaler / cameraToVertex.z ) + WIDTH/2;

	//We need to invert the height, since the image plane has its height start from above to below, but the height of an object is considered from below.
	//We could do this here or later when drawing.
	v = HEIGHT - (focalLength * ( cameraToVertex.y  * planeScaler / cameraToVertex.z) + HEIGHT/2);



	return CanvasPoint(u, v, 1.0f/cameraToVertex.z);
}
CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength, glm::vec3 vertexPosition) {
	float u;
	float v;
	float planeScaler = 160;
	//Review x and y, they maybe incorrect, even the z
	glm::vec3 cameraToVertex(0,0,0);
	// cameraToVertex.x = (vertexPosition.x - cameraPosition.x);
	// cameraToVertex.y = ( vertexPosition.y - cameraPosition.y) ;
	// cameraToVertex.z = (cameraPosition.z - vertexPosition.z);


	cameraToVertex = vertexPosition - cameraPosition;
	//cameraToVertex.x = -cameraToVertex.x;
	glm::vec3 adjustedVector = (cameraToVertex * cameraOrientation);

	//adjustedVector = cameraToVertex;
	//adjustedVector = cameraToVertex;
//	adjustedVector = cameraToVertex;
	u = focalLength * ( planeScaler * adjustedVector.x / adjustedVector.z ) + WIDTH/2;


	//We need to invert the height, since the image plane has its height start from above to below, but the height of an object is considered from below.
	//We could do this here or later when drawing.
	v = HEIGHT - (focalLength * ( planeScaler * adjustedVector.y / adjustedVector.z ) + HEIGHT/2);

	return CanvasPoint(u, v , 1/adjustedVector.z);

}

void renderClouds(DrawingWindow &window) {
	glm::vec3 cameraPosition = glm::vec3(0.0, 0.0,4.0);
	float focalLength = 2.0;
	float scaling = 0.35;
	std::vector<ModelTriangle> theTriModels = loadModel(scaling, "cornell-box.obj", "cornell-box.mtl");

	uint32_t colour = (255 << 24) + (255 << 16) + (255<< 8) + 255;


	for (ModelTriangle singleTri : theTriModels) {
		for (glm::vec3 vertex: singleTri.vertices) {
			CanvasPoint projectedPoint = projectVertexOntoCanvasPoint(cameraPosition, focalLength, vertex);
			window.setPixelColour(round(projectedPoint.x ), round(projectedPoint.y ), colour);

		}
	}
}
CanvasTriangle convert3DTriTo2D(ModelTriangle triangleIn3D, glm::vec3 cameraPosition, float focalLength) {
	CanvasPoint v1, v2, v3;
	auto vertices3D = triangleIn3D.vertices;
	v1 = projectVertexOntoCanvasPoint(cameraPosition,focalLength, vertices3D[0]);
	v2 = projectVertexOntoCanvasPoint(cameraPosition, focalLength, vertices3D[1]);
	v3 = projectVertexOntoCanvasPoint(cameraPosition, focalLength, vertices3D[2]);


	return CanvasTriangle(v1, v2, v3);
}
CanvasTriangle convert3DTriTo2D(ModelTriangle triangleIn3D, glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength) {
	CanvasPoint v1, v2, v3;
	auto vertices3D = triangleIn3D.vertices;
	v1 = projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation ,focalLength, vertices3D[0]);
	v2 = projectVertexOntoCanvasPoint(cameraPosition,cameraOrientation , focalLength, vertices3D[1]);
	v3 = projectVertexOntoCanvasPoint(cameraPosition, cameraOrientation, focalLength, vertices3D[2]);

	//Store the depth in the depth field of the respective Canvas Point (AFTER CONVERTING TO CAMERA COORDS)
	// v1.depth = 1/(cameraPosition.z -  vertices3D[0].z );
	// v2.depth = 1/(cameraPosition.z -  vertices3D[1].z );
	// v3.depth = 1/(cameraPosition.z - vertices3D[2].z);
	return CanvasTriangle(v1, v2, v3);
}