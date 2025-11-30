#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <CanvasPoint.h>
#include <Colour.h>
#include <functional>
#include <thread>
#include <TextureMap.h>
#include <iostream>
#include "ModelTriangle.h"
#include "MyFunctions/LoadingFiles.h"
#include "MyFunctions/Raytracing.h"


#include <cmath>
#define WIDTH 320
#define HEIGHT 240

void drawingALine(DrawingWindow &window, CanvasPoint start, CanvasPoint end, Colour clr) {
	uint32_t colour = (255 << 24) + (clr.red << 16) + (clr.green << 8) + clr.blue;
	float xDiff = end.x - start.x;
	float yDiff =  end.y - start.y;
	float numOfSteps = std::max(std::abs(xDiff), std::abs(yDiff));
	float xStepSize = xDiff/numOfSteps;
	float yStepSize = yDiff/numOfSteps;

	for (int  i = 0; i < std::abs(numOfSteps); i++) {
		float x = start.x + (xStepSize * i);
		float y = start.y + (yStepSize * i);
		int roundedX = round(x);
		int roundedY = round(y);
		// Check bounderies
		if (roundedX >= 0 && roundedX < WIDTH && roundedY >= 0 && roundedY < HEIGHT)
		window.setPixelColour(roundedX, roundedY, colour);

	}

}

void testDrawingALine(DrawingWindow &window) {

	drawingALine(window, CanvasPoint(0, 0), CanvasPoint(WIDTH/2, HEIGHT/2), Colour(255,200, 100));
	drawingALine(window, CanvasPoint((WIDTH - 1)/2, (HEIGHT - 1) / 2 ), CanvasPoint(WIDTH - 1, 0), Colour(255,255 , 255));
	drawingALine(window, CanvasPoint((WIDTH - 1)/2, 0), CanvasPoint((WIDTH - 1 )/2, HEIGHT), Colour(255,255 , 255));
	drawingALine(window, CanvasPoint((WIDTH - 1)/3, (HEIGHT - 1)/2), CanvasPoint((WIDTH - 1) * 2/3,(HEIGHT - 1)/ 2), Colour(255,255 , 255));

}

std::vector<float> interpolateSingleFloats(float from, float to, int numOfValues) {
	std::vector<float> result;
	float range = to - from;
	float step = range / (numOfValues - 1);

	auto nextValue = from;
	for (float y = 0; y < numOfValues; y++) {
		result.push_back(nextValue);
		nextValue += step;
	}

	return result;

}
void test_interpolateSingleFloats() {
	std::vector<float> result = interpolateSingleFloats(1, 1, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl;
}

void drawingGreyScale(DrawingWindow& window) {
	window.clearPixels();


	// uint32_t newColor = (alpha) + (r) + (g) + (b);
	// window.drawLine(0, 0, WIDTH, HEIGHT, newColors);
	// window.setPixelColour(x,y,newColor);

	std::vector<float> greyScaleRange = interpolateSingleFloats(0, 255, WIDTH);

	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < WIDTH; x++) {
			//r, g, b, have to be the same value so
			// int r, g, b = y % 256;
			//
			// int alpha = 255;
			// uint32_t newColor = (alpha << 24) + (r << 16) + (g << 8) + b;

			float red = greyScaleRange[WIDTH - x - 1];
			float green = red;
			float blue = red;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}

}


std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numOfValues) {

	std::vector<glm::vec3> result;
	std::vector<float> interpolateX = interpolateSingleFloats(from.x, to.x, numOfValues);
	std::vector<float> interpolateY = interpolateSingleFloats(from.y, to.y, numOfValues);
	std::vector<float> interpolateZ = interpolateSingleFloats(from.z, to.z, numOfValues);


	for (int i= 0; i < numOfValues; i++) {
		glm::vec3 resultI = glm::vec3(interpolateX[i], interpolateY[i], interpolateZ[i]);

		result.push_back(resultI);
	}
	return result;

}


void test_interpolateThreeElementValues() {
	glm::vec3 start(8, 2, 3);     // black
	glm::vec3 end(8, 9, 10);  // some colour
	int num = 3;

	std::vector<glm::vec3> values = interpolateThreeElementValues(start, end, num);

	std::cout << "Interpolated values from "
			  << "(" << start.x << "," << start.y << "," << start.z << ")"
			  << " to "
			  << "(" << end.x << "," << end.y << "," << end.z << ")"
			  << " in " << num << " steps:" << std::endl;

	for (int i = 0; i < values.size(); i++) {
		std::cout << i << ": ("
				  << values[i].x << ", "
				  << values[i].y << ", "
				  << values[i].z << ")"
				  << std::endl;
	}
}


void drawing2DColourInterpolation(DrawingWindow &window) {
	window.clearPixels();

	glm::vec3 topLeft(255, 0, 0);        // red
	glm::vec3 topRight(0, 0, 255);       // blue
	glm::vec3 bottomRight(0, 255, 0);    // green
	glm::vec3 bottomLeft(255, 255, 0);   // yellow

	std::vector<glm::vec3> firstColumnInterpolatedColors = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	std::vector<glm::vec3> lastColumnInterpolatedColors = interpolateThreeElementValues(topRight, bottomRight, window.height);


	for (size_t y = 0; y < window.height; y++) {

		std::vector<glm::vec3> currentRowInterpolatedColors = interpolateThreeElementValues(
			firstColumnInterpolatedColors[y], lastColumnInterpolatedColors[y], window.width);

		for (size_t x = 0; x < window.width; x++) {

			float red = currentRowInterpolatedColors[x].x;
			float green = currentRowInterpolatedColors[x].y;
			float blue = currentRowInterpolatedColors[x].z;
			//printf("%f, %f, %f\n", currentRowInterpolatedColors[x].x, currentRowInterpolatedColors[x].y, currentRowInterpolatedColors[x].z);

			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}

}

void drawingTriangleRGB(DrawingWindow &window) {

	window.clearPixels();

	for (size_t y = 0; y < window.height; y++) {



		for (size_t x = 0; x < window.width; x++) {

			glm::vec3 prox = convertToBarycentricCoordinates(
				glm::vec2(WIDTH/2, 0)
				, glm::vec2(0, HEIGHT)
				, glm::vec2(WIDTH, HEIGHT)
				, glm::vec2(x, y)
				);

			float red = prox.x * 255;
			float green = prox.z * 255;
			float blue = prox.y * 255;


			//printf("%f, %f, %f\n", currentRowInterpolatedColors[x].x, currentRowInterpolatedColors[x].y, currentRowInterpolatedColors[x].z);

			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			if (red >= 0 && green >= 0 && blue >= 0)
			window.setPixelColour(x, y, colour);
		}
	}

}



void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void drawingATriangle(DrawingWindow &window, CanvasTriangle triangle, Colour clr) {

	CanvasPoint p1 = triangle.v0();
	CanvasPoint p2 = triangle.v1();
	CanvasPoint p3 = triangle.v2();
	drawingALine(window, p1, p2, clr);
	drawingALine(window, p2, p3, clr);
	drawingALine(window, p1, p3, clr);

}

//v0 has the be the highest point of the three (largest y)
void fillingHalfTriangle(DrawingWindow &window, CanvasTriangle tri, Colour clr) {
	uint32_t fillColour = (255 << 24) + (clr.red << 16) + (clr.green << 8) + clr.blue;

	CanvasPoint origin = tri.v0(); // bottom vertex
	CanvasPoint left = tri.v1();
	CanvasPoint right = tri.v2();

	if (left.x > right.x) std::swap(left, right);

	float yMiddle = right.y;
	float yOrigin = origin.y;
	bool isFlatTop = yMiddle <= yOrigin;
	// int yStart = (int)std::ceil(yMiddle);
	// int yEnd = (int)std::floor(yOrigin);
	//Avoiding the floating point error, or the repeating y problem.
	float yStartTemp = yMiddle;
	float yEndTemp = yOrigin;
	if (!isFlatTop) std::swap(yStartTemp, yEndTemp);
	int yStart = std::ceil(yStartTemp);
	int yEnd = std::floor(yEndTemp);

	for (int y = yStart; y <= yEnd; ++y) {
		float t, xLeft, xRight;
		if (isFlatTop) //True if it is the flat top
		{
			t = (y - yMiddle) / (yOrigin - yMiddle);
			xLeft  = left.x + (origin.x - left.x) * t;
			xRight = right.x + (origin.x - right.x) * t;
		}
		else {
			t = (y - origin.y) /(yMiddle - yOrigin);
			xLeft  = origin.x + (left.x - origin.x) * t;
			xRight = origin.x + (right.x - origin.x) * t;

		}

		if (xLeft > xRight) std::swap(xLeft, xRight);

		int xStart = std::ceil(xLeft);
		int xEnd = std::floor(xRight);
		for (int x = xStart; x <= xEnd; ++x)
			window.setPixelColour(x, y, fillColour);
	}
}

void drawingFilledTriangles(DrawingWindow &window, CanvasTriangle triangle, Colour clr) {

	drawingATriangle(window, triangle, clr); //drawing the outline

	CanvasPoint v0 = triangle.v0();
	CanvasPoint v1 = triangle.v1();
	CanvasPoint v2 = triangle.v2();

	if (v0.y > v1.y) std::swap(v0, v1);
	if (v0.y > v2.y) std::swap(v0, v2);
	if (v1.y > v2.y) std::swap(v1, v2);
	float xOfDividerPoint = ((v1.y - v0.y) * (v2.x - v0.x)/(v2.y - v0.y) ) + v0.x;

	CanvasPoint dividerPoint  = CanvasPoint(xOfDividerPoint, v1.y);
	CanvasTriangle tri1 = CanvasTriangle(v0, dividerPoint, v1);
	CanvasTriangle tri2 = CanvasTriangle(v2, dividerPoint, v1);
	fillingHalfTriangle(window, tri1, clr);
	fillingHalfTriangle(window, tri2, clr);


}

void testFillingATriangle(DrawingWindow &window) {
	//Small triangle Example:

	// canvaspoint p1 = canvaspoint(20, 20);
	// canvaspoint p2 = canvaspoint(100, 30);
	// canvaspoint p3 = canvaspoint(50, 100);
	// canvastriangle tri = canvastriangle(p1, p2, p3);
	// drawingatriangle(window, tri, colour(255, 255, 255));


	int randX = rand() % WIDTH;
	int randY = rand() % HEIGHT;

	//	printf("x is %i, y is %i \n", randX, randY);
	CanvasPoint p1 = CanvasPoint(randX, randY);
	randX = rand() % WIDTH;
	randY = rand() % HEIGHT;

	CanvasPoint p2 = CanvasPoint(randX, randY);
	randX = rand() % WIDTH;
	randY = rand() % HEIGHT;
	CanvasPoint p3 = CanvasPoint(randX, randY);
	CanvasTriangle tri = CanvasTriangle(p1, p2, p3);
	drawingFilledTriangles(window, tri, Colour(rand() %  256,rand() %  256,rand() %  256));


}
void testDrawingATriangle(DrawingWindow &window) {
	//Small triangle Example:

	// canvaspoint p1 = canvaspoint(20, 20);
	// canvaspoint p2 = canvaspoint(100, 30);
	// canvaspoint p3 = canvaspoint(50, 100);
	// canvastriangle tri = canvastriangle(p1, p2, p3);
	// drawingatriangle(window, tri, colour(255, 255, 255));


	int randX = rand() % WIDTH;
	int randY = rand() % HEIGHT;

	//	printf("x is %i, y is %i \n", randX, randY);
	CanvasPoint p1 = CanvasPoint(randX, randY);
	randX = rand() % WIDTH;
	randY = rand() % HEIGHT;

	CanvasPoint p2 = CanvasPoint(randX, randY);
	randX = rand() % WIDTH;
	randY = rand() % HEIGHT;
	CanvasPoint p3 = CanvasPoint(randX, randY);
	CanvasTriangle tri = CanvasTriangle(p1, p2, p3);
	drawingATriangle(window, tri, Colour(rand() %  256,rand() %  256,rand() %  256));



}

void textureHalfTriangle(DrawingWindow &window, CanvasTriangle theTri, CanvasTriangle textureTri, std::vector<std::vector<uint32_t>> textureArray) {
	CanvasPoint origin = theTri.v0(); // bottom vertex
	CanvasPoint left = theTri.v1();
	CanvasPoint right = theTri.v2();
	CanvasPoint textureOrigin = textureTri.v0(); // bottom vertex
	CanvasPoint textureLeft = textureTri.v1();
	CanvasPoint textureRight = textureTri.v2();

	if (left.x > right.x) std::swap(left, right);
	if (textureLeft.x > textureRight.x) std::swap(textureLeft, textureRight);

	float yMiddle = right.y;
	float yOrigin = origin.y;
	bool isFlatTop = yMiddle <= yOrigin;
	int yStart = std::ceil(yMiddle) ;
	int yEnd = std::floor(yOrigin);
	if (!isFlatTop) std::swap(yStart, yEnd);

	for (int y = yStart; y <= yEnd; ++y) {
		float t, xLeft, xRight, texture_xLeft, texture_xRight, texture_yLeft, texture_yRight;
		if (isFlatTop) //True if it is the flat top
		{
			t = (y - yMiddle) / (yOrigin - yMiddle);
			xLeft  = left.x + (origin.x - left.x) * t;
			xRight = right.x + (origin.x - right.x) * t;


			texture_yLeft = textureLeft.y  + t * ( textureOrigin.y - textureLeft.y);
			texture_yRight = textureRight.y + t * ( textureOrigin.y - textureRight.y);


			texture_xLeft = textureLeft.x + (textureOrigin.x - textureLeft.x) * t;
			texture_xRight = textureRight.x + ( textureOrigin.x - textureRight.x) * t;

		}
		else {
			t = (y - origin.y) /(yMiddle - yOrigin);
			xLeft  = origin.x + (left.x - origin.x) * t;
			xRight = origin.x + (right.x - origin.x) * t;


			texture_yLeft = textureOrigin.y + t * ( textureLeft.y - textureOrigin.y);
			texture_yRight = textureOrigin.y + t * ( textureRight.y - textureOrigin.y);

			texture_xLeft = textureOrigin.x + (textureLeft.x - textureOrigin.x) * t;
			texture_xRight = textureOrigin.x + (textureRight.x - textureOrigin.x) * t;
		}

		int xStart = std::ceil(xLeft);
		int xEnd = std::floor(xRight);

		for (int x = xStart; x <= xEnd; ++x){
			if (xEnd == xStart) continue; // avoid divide-by-zero

			float xRatio = static_cast<float>(x - xStart) / static_cast<float>(xEnd - xStart);
			float xPoint = texture_xLeft + (texture_xRight - texture_xLeft) * xRatio;
			float yPoint = texture_yLeft + xRatio * (texture_yRight - texture_yLeft);

			window.setPixelColour(x, y, textureArray[round(yPoint)][round(xPoint)]);
		}
	}
}
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle theTri, TextureMap &textureFile) {

	std::vector<std::vector<uint32_t>> textureArray ;


	//creating the 2D pixels array;
	int height = static_cast<int>(textureFile.height);
	int width = static_cast<int>(textureFile.width);
	for (int y = 0; y < height; y++) {
		std::vector<uint32_t> rowToAdd;
		for (int x = 0; x < width; x++) {

			int index = y * width + x;
			rowToAdd.push_back(textureFile.pixels.at(index));
		}
		textureArray.push_back(rowToAdd);

	}
	drawingATriangle(window, theTri, Colour(255, 255, 255)); //drawing the outline

	CanvasPoint v0 = theTri.v0();
	CanvasPoint v1 = theTri.v1();
	CanvasPoint v2 = theTri.v2();

	if (v0.y > v1.y) std::swap(v0, v1);
	if (v0.y > v2.y) std::swap(v0, v2);
	if (v1.y > v2.y) std::swap(v1, v2);
	float xOfDividerPoint = ((v1.y - v0.y) * (v2.x - v0.x)/(v2.y - v0.y) ) + v0.x;
	CanvasPoint dividerPoint  = CanvasPoint(xOfDividerPoint, v1.y);

	//float xOfTextureDividerPoint = ((v1.texturePoint.y - v0.texturePoint.y) * (v2.texturePoint.x - v0.texturePoint.x)/(v2.texturePoint.y - v0.texturePoint.y) ) + v0.texturePoint.x;
	float textureRatio_y = (dividerPoint.y - v0.y) / (v2.y - v0.y);
	float dividerPoint_y = v0.texturePoint.y + textureRatio_y * (v2.texturePoint.y - v0.texturePoint.y);

	float textureRatio_x = (xOfDividerPoint - v0.x) / (v2.x - v0.x);
	float dividerPoint_x = v0.texturePoint.x + textureRatio_x * (v2.texturePoint.x - v0.texturePoint.x);
	CanvasPoint dividerPointInTexture = CanvasPoint(dividerPoint_x, dividerPoint_y);

	CanvasTriangle tri1 = CanvasTriangle(v0, dividerPoint, v1);
	CanvasTriangle tri2 = CanvasTriangle(v2, v1, dividerPoint);
	CanvasPoint t0 = CanvasPoint(v0.texturePoint.x, v0.texturePoint.y );
	CanvasPoint t1 = CanvasPoint(v1.texturePoint.x, v1.texturePoint.y );
	CanvasPoint t2 = CanvasPoint(v2.texturePoint.x, v2.texturePoint.y );
	CanvasTriangle textureTri = CanvasTriangle(t0, dividerPointInTexture, t1);
	CanvasTriangle textureTri2= CanvasTriangle(t2, t1, dividerPointInTexture);
	textureHalfTriangle(window, tri1, textureTri, textureArray);
	textureHalfTriangle(window, tri2, textureTri2, textureArray);

}
void testATexturedTri(DrawingWindow &window) {
	TextureMap textureFile = TextureMap("build/texture.ppm");
	CanvasPoint p1 = CanvasPoint(160, 10);
	CanvasPoint p2 = CanvasPoint(300, 230);
	CanvasPoint p3 = CanvasPoint(10, 150);
	p1.texturePoint = TexturePoint(195, 5);
	p2.texturePoint = TexturePoint(395, 380);
	p3.texturePoint = TexturePoint(65, 330);

	CanvasTriangle exampleTri = CanvasTriangle(p1, p2, p3);
	drawTexturedTriangle(window, exampleTri, textureFile);
}
void rotateAroundX(glm::vec3 &cameraPosition, float degrees) {
	float radians = degrees * M_PI / 180.0f;

	glm::mat3 xRotationMatrix (
		1, 0, 0,
		0, std::cos(radians), std::sin(radians),
		0, -std::sin(radians), std::cos(radians)
		);
    cameraPosition = (xRotationMatrix * cameraPosition);
}
void rotateAroundY(glm::vec3 &cameraPosition, float degrees) {
	float radians = degrees * M_PI / 180.0f;

	glm::mat3 yRotationMatrix (
		std::cos(radians), 0, -std::sin(radians),
		0, 1,0,
		std::sin(radians), 0, std::cos(radians)
		);
	cameraPosition = (yRotationMatrix * cameraPosition);
}

void tilt(glm::mat3 &cameraOrientation, float degrees) {
	float radians = degrees * M_PI / 180.0f;

	glm::mat3 xRotationMatrix (
		1, 0, 0,
		0, std::cos(radians), std::sin(radians),
		0, -std::sin(radians), std::cos(radians)
		);



	cameraOrientation = xRotationMatrix * cameraOrientation;
}

void pan(glm::mat3 &cameraOrientation, float degrees) {
	float radians = degrees * M_PI / 180.0f;

	glm::mat3 yRotationMatrix (
		std::cos(radians), 0, -std::sin(radians),
		0, 1,0,
		std::sin(radians), 0, std::cos(radians)
		);



	cameraOrientation = yRotationMatrix * cameraOrientation;
}





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
	std::vector<ModelTriangle> theTriModels = loadModel(scaling);

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
void renderSketchedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength ) {
	window.clearPixels();
	for (ModelTriangle triIn3D : theTriModels) {
		CanvasTriangle triIn2D = convert3DTriTo2D(triIn3D, cameraPosition, cameraOrientation, focalLength);
		drawingATriangle(window, triIn2D, Colour(255, 255, 255));
	}

}

void fillTriangleWithDepth(DrawingWindow &window, std::vector<std::vector<float>> &zBuffer, CanvasTriangle tri, Colour colour) {
	uint32_t colourAsInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
	// std::vector<CanvasPoint> pointsToUpdate;
	CanvasPoint v0 = tri.v0();
	CanvasPoint v1 = tri.v1();
	CanvasPoint v2 = tri.v2();

	if (v0.y > v1.y) std::swap(v0, v1);
	if (v0.y > v2.y) std::swap(v0, v2);
	if (v1.y > v2.y) std::swap(v1, v2);



	float cross = (v1.x - v0.x)*(v2.y - v0.y) - (v1.y - v0.y)*(v2.x - v0.x);
	if (cross < 0) std::swap(v0, v2); // ensure consistent winding

	for (int y = 0; y< HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			glm::vec3 prox = convertToBarycentricCoordinates(
			glm::vec2(v0.x, v0.y),
			glm::vec2(v1.x, v1.y),
			glm::vec2(v2.x, v2.y),
				glm::vec2(x, y));
			//printf("%f, %f, %f, %f \n", prox.x, prox.y, prox.z, (prox.x + prox.y+prox.z));

			if (prox.x >= 0 && prox.y >= 0 && prox.z >= 0) {

				float currentDepth = prox.x * v1.depth + prox.y *v2.depth + prox.z * v0.depth;

				//printf("%f\n", currentDepth);
				if (currentDepth > zBuffer[y][x]) {
					zBuffer[y][x]  = currentDepth;
					window.setPixelColour(round(x), round(y), colourAsInt);
				}

			}
		}
	}
}
void renderRasterizedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength ) {

	window.clearPixels();
	std::vector<std::vector<float>> zBuffer;
	for (int y = 0; y < HEIGHT; y++ ) {
		std::vector<float> depthRow;
		for (int x = 0; x < WIDTH; x++) {
			depthRow.push_back(0);
		}
			zBuffer.push_back(depthRow);
	}



	#pragma omp parallel for
	for (ModelTriangle triIn3D : theTriModels) {
		CanvasTriangle triIn2D = convert3DTriTo2D(triIn3D,  cameraPosition, cameraOrientation, focalLength);
		//drawingFilledTriangles(window, triIn2D, color);
		fillTriangleWithDepth(window,zBuffer, triIn2D, triIn3D.colour);
	}

}
void lookAt(glm::vec3 center, glm::mat3 &cameraOrientation, glm::vec3 cameraPosition) {

	glm::vec3 forward, right, up;
	glm::vec3 vertical(0, 1, 0);


	forward =glm::normalize(center - cameraPosition) ;

	right = glm::cross(forward, vertical);
	right = glm::normalize(right);
	up = glm::cross(right, forward);


	cameraOrientation[0] = right;
	cameraOrientation[1] = up;
	cameraOrientation[2] = forward;

}
#include <unistd.h>

void orbit(DrawingWindow &drawing_window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, bool &orbitStatus){
	drawing_window.clearPixels();

	// glm::vec3 stopPosition(0, 0, 0);
	// if (start) {
	// start = false;
	// }
	glm::vec3 center(0,0,0);
	if (orbitStatus) {
		rotateAroundY(cameraPosition, +60);
		lookAt(center, cameraOrientation, cameraPosition);
		usleep(500000);
	}


}
void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, bool &orbitStatus, int &renderMode) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) cameraPosition.x -= 0.5; else if (event.key.keysym.sym == SDLK_RIGHT) cameraPosition.x += 0.5;
		else if (event.key.keysym.sym == SDLK_UP) cameraPosition.y += 0.5; else if (event.key.keysym.sym == SDLK_DOWN) cameraPosition.y -= 0.5;
		else if (event.key.keysym.sym == SDLK_w) cameraPosition.z += 0.5; else if (event.key.keysym.sym == SDLK_s) cameraPosition.z -= 0.5;
		else if (event.key.keysym.sym == SDLK_u) testDrawingATriangle(window);
		else if (event.key.keysym.sym == SDLK_f) testFillingATriangle(window);
		else if (event.key.keysym.sym == SDLK_x) rotateAroundX(cameraPosition, 10);
		else if (event.key.keysym.sym == SDLK_y) rotateAroundY(cameraPosition, 10);
		else if (event.key.keysym.sym == SDLK_j) tilt(cameraOrientation, -10); else if (event.key.keysym.sym == SDLK_k) tilt(cameraOrientation, +10);
		else if (event.key.keysym.sym == SDLK_h) pan(cameraOrientation, -10); else if (event.key.keysym.sym == SDLK_l) pan(cameraOrientation, +10);
		else if (event.key.keysym.sym == SDLK_o)  orbitStatus = !orbitStatus;

		else if (event.key.keysym.sym == SDLK_0) renderMode = 0;
		else if (event.key.keysym.sym == SDLK_1) renderMode = 1;
		else if (event.key.keysym.sym == SDLK_2) renderMode = 2;
		else if (event.key.keysym.sym == SDLK_3) renderMode = 3;

	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");

		int frameNumber = 0;
		std::string filename = "frame_" + std::to_string(frameNumber) + ".bmp";
		window.saveBMP(filename);
		window.saveBMP("output.bmp");
	}

	window.clearPixels();
}

#include <iomanip> // Required for std::setw and setfill
#include <sstream>
void generateAnimation(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels,
                       glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                       glm::vec3 &lightSourcePosition, float &focalLength) {

    int totalFrames = 720; // 30 seconds * 24 fps
    int renderMode = 1;    // Start in Sketch mode

    std::cout << "Starting render of " << totalFrames << " frames..." << std::endl;

    for (int i = 0; i < totalFrames; i++) {

        // --- ANIMATION LOGIC ---

        // PHASE 1: 0 to 5 seconds (Wireframe Zoom Out)
        if (i < 120) {
            renderMode = 1;
            // Move camera back slowly (Simulating 'S' key)
            cameraPosition.z += 0.02f;
        }
        // PHASE 2: 5 to 15 seconds (Rasterized Orbit)
        else if (i < 360) {
            renderMode = 2;
            // Orbit camera (Simulating 'Y' key but smoother)
            // Assuming your rotate function takes degrees, we use small steps for smooth video
            rotateAroundY(cameraPosition, 0.5f);

            // Also lift the camera up slightly
            cameraPosition.y += 0.01f;

            // Look down slightly to keep object in center (Simulating 'J' tilt)
            tilt(cameraOrientation, -0.1f);
        }
        // PHASE 3: 15 to 30 seconds (Raytraced Cinematic Pan)
        else {
            renderMode = 3;
            // Very slow, high-quality movement
            // Pan camera slowly to the right (Simulating 'L' key)
            pan(cameraOrientation, 0.1f);

            // Optional: Move light source to create shifting shadows
            lightSourcePosition.x += 0.05f;
        }

        // --- RENDER STEP ---

        window.clearPixels(); // Clear buffer before drawing

        if (renderMode == 1) {
            renderSketchedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
        }
        else if (renderMode == 2) {
            renderRasterizedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
        }
        else if (renderMode == 3) {
            renderRaytracedModelWithShadows(window, theTriModels, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
        }

        // --- SAVE FRAME ---

        // Create filename like "output_0001.bmp", "output_0002.bmp"
        std::stringstream ss;
        ss << "output_" << std::setw(4) << std::setfill('0') << i << ".bmp";

        window.saveBMP(ss.str());

        // Progress update to console
        std::cout << "Rendered frame " << i << "/" << totalFrames << " (Mode: " << renderMode << ")" << std::endl;
    }

    std::cout << "Animation Render Complete!" << std::endl;
}
int main(int argc, char *argv[]) {

//	 test_interpolateSingleFloats();
//	 test_interpolateThreeElementValues();

	int renderMode = 0;
	glm::vec3 cameraPosition = glm::vec3(0.0, 0.0,4.0);
	glm::vec3 lightSourcePosition = glm::vec3(0.0, 2.5 * 0.35,0.0);

	glm::mat3 cameraOrientation (
		1, 0, 0,
		0, 1, 0,
		0, 0, -1

		);
	float focalLength = 2.0;
	float scaling = 0.35;
	//test_loadModel();
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

	std::vector<ModelTriangle> theTriModels = loadModel(scaling);

	//A variable to keep track of the orbiting status of the function 'orbit()';
	bool orbitStatus;
	// generateAnimation(window, theTriModels, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
	SDL_Event event;

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation, orbitStatus, renderMode);
		//draw(window);
		//drawingGreyScale(window);
		//drawing2DColourInterpolation(window);
		//drawingTriangleRGB(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		//testATexturedTri(window);
	//	testDrawingALine(window);
	//	testDrawingATriangle(window);
		//renderClouds(window);
		//renderSketchedModel(window);


		//renderRasterizedModel(window, cameraPosition, cameraOrientation, focalLength, scaling);


		//testGetClosestValidIntersection();

		orbit(window, cameraPosition, cameraOrientation, orbitStatus);
		//renderRaytracedModel(window, cameraPosition, cameraOrientation, focalLength, scaling);
		if (renderMode == 1) renderSketchedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
		else if (renderMode == 2) renderRasterizedModel(window,theTriModels, cameraPosition, cameraOrientation, focalLength );
		else if (renderMode == 3) {
			renderRaytracedModelWithShadows(window, theTriModels, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
			// renderRaytracedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
		//	renderMode = 0;

		}
		window.renderFrame();


	}
}