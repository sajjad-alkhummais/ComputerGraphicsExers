//
// Created by sajjad on 11/30/25.

#include "Texturing.h"

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

std::vector<std::vector<uint32_t>> createTextureArray(TextureMap &textureFile) {
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
	return textureArray;
}
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle theTri, TextureMap &textureFile) {

	std::vector<std::vector<uint32_t>> textureArray = createTextureArray(textureFile);

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
	TextureMap textureFile = TextureMap("ModelsFiles/texture.ppm");
	CanvasPoint p1 = CanvasPoint(160, 10);
	CanvasPoint p2 = CanvasPoint(300, 230);
	CanvasPoint p3 = CanvasPoint(10, 150);
	p1.texturePoint = TexturePoint(195, 5);
	p2.texturePoint = TexturePoint(395, 380);
	p3.texturePoint = TexturePoint(65, 330);

	CanvasTriangle exampleTri = CanvasTriangle(p1, p2, p3);
	drawTexturedTriangle(window, exampleTri, textureFile);
}

uint32_t getTextureColourOfIntersection(std::vector<std::vector<uint32_t>> &textureArray, ModelTriangle intersectedTriangle, float u, float v){
	float w = 1.0f - u - v;

	glm::vec2 topPoint = glm::vec2(intersectedTriangle.texturePoints[1].x, intersectedTriangle.texturePoints[1].y);
	glm::vec2 leftPoint = glm::vec2(intersectedTriangle.texturePoints[0].x, intersectedTriangle.texturePoints[0].y);
	glm::vec2 rightPoint = glm::vec2(intersectedTriangle.texturePoints[2].x, intersectedTriangle.texturePoints[2].y);
	glm::vec2 leftEdge = topPoint - leftPoint;
	glm::vec2 bottomEdge = rightPoint - leftPoint;
	glm::vec2 targetPointVector = leftEdge * u + bottomEdge * v;
	targetPointVector = leftPoint * u + topPoint * v + rightPoint * w;
	int target_x = round(targetPointVector.x);
	int target_y = round(targetPointVector.y);

	return textureArray[target_y][target_x];

}