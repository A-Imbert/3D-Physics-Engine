#pragma once
#include <glm/glm.hpp>
class Transform {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Transform(glm::vec3 t_position, glm::vec3 t_rotation, glm::vec3 t_scale) {
		position = t_position;
		rotation = t_rotation;
		scale = t_scale;
	}
};