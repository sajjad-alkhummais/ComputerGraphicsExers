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
#include "MyFunctions/Wireframe.h"
#include "MyFunctions/Texturing.h"
#include "MyFunctions/CameraMovements.h"
#include "MyFunctions/Projection.h"


#include <cmath>
#define WIDTH 320
#define HEIGHT 240










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

void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, bool &orbitStatus, int &renderMode, glm::vec3 &lightSourcePosition) {

	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) cameraPosition.x -= 0.5; else if (event.key.keysym.sym == SDLK_RIGHT) cameraPosition.x += 0.5;
		else if (event.key.keysym.sym == SDLK_UP) cameraPosition.y += 0.5; else if (event.key.keysym.sym == SDLK_DOWN) cameraPosition.y -= 0.5;
		else if (event.key.keysym.sym == SDLK_w) cameraPosition.z += 0.5; else if (event.key.keysym.sym == SDLK_s) cameraPosition.z -= 0.5;

		if (event.key.keysym.sym == SDLK_5) lightSourcePosition.x -= 0.5; else if (event.key.keysym.sym == SDLK_6) lightSourcePosition.x += 0.5;
		else if (event.key.keysym.sym == SDLK_7) lightSourcePosition.y += 0.5; else if (event.key.keysym.sym == SDLK_8) lightSourcePosition.y -= 0.5;
		else if (event.key.keysym.sym == SDLK_0) lightSourcePosition.z += 0.5; else if (event.key.keysym.sym == SDLK_9) lightSourcePosition.z -= 0.5;

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
void generateAnimation(DrawingWindow &window, std::vector<ModelTriangle> &theTriModels, std::vector<std::vector<uint32_t>> textureArray,
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
            renderRaytracedModelWithShadows(window, theTriModels, textureArray, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
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

	//Load models and textures
	std::vector<ModelTriangle> theTriModels = loadModel(scaling, "cornell-box.obj", "cornell-box.mtl");
	// std::vector<ModelTriangle> theTriModels = loadModel(scaling, "textured-cornell-box.obj", "textured-cornell-box.mtl");
	TextureMap textureFile = TextureMap("ModelsFiles/texture.ppm");
	std::vector<std::vector<uint32_t>> textureArray = createTextureArray(textureFile);


	//A variable to keep track of the orbiting status of the function 'orbit()';
	bool orbitStatus = false;
	// generateAnimation(window, theTriModels, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
	SDL_Event event;

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation, orbitStatus, renderMode, lightSourcePosition);
		//draw(window);
		//drawingGreyScale(window);
		//drawing2DColourInterpolation(window);
		//drawingTriangleRGB(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		// testATexturedTri(window);
	//	testDrawingALine(window);
	//	testDrawingATriangle(window);
		//renderClouds(window);
		//renderSketchedModel(window);


		//renderRasterizedModel(window, cameraPosition, cameraOrientation, focalLength, scaling);


		// test_loadUntexturedModel();
		//testGetClosestValidIntersection();

		orbit(window, cameraPosition, cameraOrientation, orbitStatus);
		if (renderMode == 1) renderSketchedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
		else if (renderMode == 2) renderRasterizedModel(window,theTriModels, cameraPosition, cameraOrientation, focalLength );
		else if (renderMode == 3) {
		 	renderRaytracedModelWithShadows(window, theTriModels, textureArray, cameraPosition, cameraOrientation, lightSourcePosition, focalLength);
		 	// renderRaytracedModel(window, theTriModels, cameraPosition, cameraOrientation, focalLength);
		 //	renderMode = 0;

		 }
		window.renderFrame();


	}
}