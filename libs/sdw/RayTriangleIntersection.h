#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "ModelTriangle.h"
#include <vector>

struct RayTriangleIntersection {
	glm::vec3 intersectionPoint;
	float distanceFromCamera;
	ModelTriangle intersectedTriangle;
	size_t triangleIndex;
	uint32_t textureColourAsInt;
	bool intersectionFound;
	glm::vec3 barycentericValues;
	std::vector<ModelTriangle> thisAndAdjacentTriangles;
	RayTriangleIntersection();
	RayTriangleIntersection(const glm::vec3 &point, float distance, const ModelTriangle &triangle, size_t index);
	friend std::ostream &operator<<(std::ostream &os, const RayTriangleIntersection &intersection);
};
