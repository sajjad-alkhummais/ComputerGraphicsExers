//
// Created by sajjad on 11/30/25.
//
#include "Wireframe.h"



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

	for (size_t i = 0; i < values.size(); i++) {
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




void renderSketchedModel(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength ) {
	// window.clearPixels();
	for (ModelTriangle triIn3D : theTriModels) {
		CanvasTriangle triIn2D = convert3DTriTo2D(triIn3D, cameraPosition, cameraOrientation, focalLength);
		Colour col = triIn3D.colour;
		drawingATriangle(window, triIn2D, col);
	}

}
