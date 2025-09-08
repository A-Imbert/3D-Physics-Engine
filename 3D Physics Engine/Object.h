#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Collider.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Objects.h"
#include "Camera.h"
#include "ObjectRenderer.h"
class Object {
public:
	Transform* transform;
	glm::vec3 colour;
	Rigidbody* rb;
	Collider* collider;
	ShapeType type;

	Object(ShapeType t_type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 t_colour, float mass, bool hasRigidBody = true) {
		colour = t_colour;
		transform = new Transform(position, rotation, scale);
		if (hasRigidBody) {
			rb = new Rigidbody(mass, this, true);
		}
		type = t_type;
	}
	void Update(float deltaTime) {
		rb->Integrate(deltaTime);
	}
	void Draw(ObjectRenderer& renderer, Camera& camera) {
		glm::mat4 model = getModelMatrix();
		renderer.Draw(model, colour, camera);
	}
	glm::mat4 getModelMatrix() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform->position);
		model = glm::rotate(model, transform->rotation.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, transform->rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, transform->rotation.z, glm::vec3(0, 0, 1));
		model = glm::scale(model, transform->scale);
		return model;
	}
	void UpdateCollider() {
		if (collider) collider->Update();
	}
	void setCollider(Collider* collider) {
		this->collider = collider;
	}
	const AABB& GetAABB() const {
		return collider->GetAABB();
	}

};