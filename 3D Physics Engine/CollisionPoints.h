#pragma once
#include <glm/glm.hpp>
class CollisionPoints {
public:
	glm::vec3 normal;
	float penetrationDepth;
	bool hasCollision;
};