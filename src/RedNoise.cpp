#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <CanvasPoint.h>
#include <Colour.h>
#include <thread>
#include <TextureMap.h>
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
		//std::cout << CanvasPoint(x, y) << std::endl;
		window.setPixelColour(round(x), round(y), colour);
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
	std::vector<float> result;
	result = interpolateSingleFloats(1, 1, 7);
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

#include <algorithm>
#include <cmath>

static float lerp(float a, float b, float t) {
	return a + t * (b - a);
}


void fillingHalfTriangle(DrawingWindow &window, CanvasTriangle tri, Colour clr) {
	uint32_t fillColour = (255 << 24) + (clr.red << 16) + (clr.green << 8) + clr.blue;

	CanvasPoint origin = tri.v0(); // bottom vertex
	CanvasPoint left = tri.v1();
	CanvasPoint right = tri.v2();

	if (left.x > right.x) std::swap(left, right);

	float yMiddle = right.y;
	float yOrigin = origin.y;
	bool isFlatTop = yMiddle <= yOrigin;
	int yStart = std::ceil(yMiddle) ;
	int yEnd = std::floor(yOrigin);
	if (!isFlatTop) std::swap(yStart, yEnd);

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
	CanvasTriangle tri2 = CanvasTriangle(v2, v1, dividerPoint);
//
	fillingHalfTriangle(window, tri1, clr);
	fillingHalfTriangle(window, tri2, clr);

// 	CanvasPoint leftPointFirstTriangle;
// 	CanvasPoint rightPointFirstTriangle;
// 	if (dividerPoint.x < v1.x) {
// 		rightPointFirstTriangle = v1;
// 		leftPointFirstTriangle = dividerPoint;
// 	}
// 	else {
// 		rightPointFirstTriangle = dividerPoint;
// 		leftPointFirstTriangle = v1;
// 	}
// // draw the right line of the first triangle
// 	clr = Colour(255, 255, 255);
// 	uint32_t lineColour = (255 << 24) + (clr.red << 16) + (clr.green << 8) + clr.blue;
// 	float x2Diff = rightPointFirstTriangle.x - v0.x;
// 	float y2Diff =  rightPointFirstTriangle.y - v0.y;
// 	float numOfSteps2 = std::max(std::abs(x2Diff), std::abs(y2Diff));
// 	float xStepSize2 = x2Diff/numOfSteps2;
// 	float yStepSize2 = y2Diff/numOfSteps2;
// 	std::vector<float> xCdOfRightLine;
// 	for (int  i = 0; i < std::abs(numOfSteps2); i++) {
// 		float x = v0.x + (xStepSize2 * i);
// 		float y = v0.y + (yStepSize2 * i);
// 		xCdOfRightLine.push_back(round(x));
// 		//std::cout << CanvasPoint(x, y) << std::endl;
// 		window.setPixelColour(round(x), round(y), lineColour);
// 	}
//
// 	clr = Colour(0, 0, 255);
//
// 	lineColour = (255 << 24) + (clr.red << 16) + (clr.green << 8) + clr.blue;
// // draw the left line of the first triangle
// 	float xDiff = leftPointFirstTriangle.x - v0.x;
// 	float yDiff =  leftPointFirstTriangle.y - v0.y;
// 	float numOfSteps = std::max(std::abs(xDiff), std::abs(yDiff));
// 	float xStepSize = xDiff/numOfSteps;
// 	float yStepSize = yDiff/numOfSteps;
//
// 	for (int  i = 0; i < std::abs(numOfSteps); i++) {
// 		float x = v0.x + (xStepSize * i);
// 		float y = v0.y + (yStepSize * i);
//
// 		window.setPixelColour(round(x), round(y), lineColour);
// 		//filling up to the right line
// 		int j = round(x) + 1;
// 		int xToStopAt = round(xCdOfRightLine[y - v0.y]);
// 		while (j < xToStopAt){
// 			window.setPixelColour(j, round(y), fillingColour);
// 			j++;
// 		}
// 	}

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
void textureHalfTriangle(DrawingWindow &window, CanvasTriangle theTri, CanvasTriangle textureTri, std::vector<std::vector<float>> textureArray) {


}
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle theTri, TextureMap &textureFile) {

	std::vector<std::vector<float>> textureArray ;


	//creating the 2D pixels array;
	int height = static_cast<int>(textureFile.height);
	int width = static_cast<int>(textureFile.width);
	for (int y = 0; y < height; y++) {
		std::vector<float> rowToAdd;
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
	float xOfTextureDividerPoint = ((v1.texturePoint.y - v0.texturePoint.y) * (v2.texturePoint.x - v0.texturePoint.x)/(v2.texturePoint.y - v0.texturePoint.y) ) + v0.texturePoint.x;

	CanvasPoint dividerPoint  = CanvasPoint(xOfDividerPoint, v1.y);
	dividerPoint.texturePoint = TexturePoint(xOfTextureDividerPoint, v1.texturePoint.y);
	CanvasPoint dividerPointInTexture = CanvasPoint(dividerPoint.texturePoint.x, dividerPoint.texturePoint.y);
	CanvasTriangle tri1 = CanvasTriangle(v0, dividerPoint, v1);

	CanvasTriangle tri2 = CanvasTriangle(v2, v1, dividerPoint);
	CanvasPoint t0 = CanvasPoint(v0.texturePoint.x, v0.texturePoint.y );
	CanvasPoint t1 = CanvasPoint(v1.texturePoint.x, v1.texturePoint.y );
	CanvasPoint t2 = CanvasPoint(v2.texturePoint.x, v2.texturePoint.y );
	CanvasTriangle tri1InTexture = CanvasTriangle(t0, t1, dividerPointInTexture);
	CanvasTriangle tri2Intexture = CanvasTriangle(t2, t1, dividerPointInTexture);

}
void testATexturedTri(DrawingWindow &window) {
	TextureMap textureFile = TextureMap("texture.ppm");
	CanvasPoint p1 = CanvasPoint(160, 10);
	CanvasPoint p2 = CanvasPoint(300, 230);
	CanvasPoint p3 = CanvasPoint(10, 150);
	p1.texturePoint = TexturePoint(195, 5);
	p2.texturePoint = TexturePoint(395, 380);
	p3.texturePoint = TexturePoint(65, 330);

	CanvasTriangle exampleTri = CanvasTriangle(p1, p2, p3);
	drawTexturedTriangle(window, exampleTri, textureFile);
}
void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl; else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) testDrawingATriangle(window);
		else if (event.key.keysym.sym == SDLK_f) testFillingATriangle(window);
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}

}

int main(int argc, char *argv[]) {

//	 test_interpolateSingleFloats();
//	 test_interpolateThreeElementValues();

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		//drawingGreyScale(window);
		//drawing2DColourInterpolation(window);
		//drawingTriangleRGB(window);
		Colour white = Colour(255,255 , 255);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
	//	testDrawingALine(window);
	//	testDrawingATriangle(window);
		window.renderFrame();

	}
}