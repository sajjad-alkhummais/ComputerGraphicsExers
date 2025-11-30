//
// Created by sajjad on 11/30/25.
//

#include "CameraMovements.h"


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
