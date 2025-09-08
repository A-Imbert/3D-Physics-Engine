#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "Transform.h"

class Object; // Forward declaration

class Rigidbody {
public:
    // Linear motion
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 accumForce = glm::vec3(0.0f);
    float mass;
    float inverseMass;
    float restitution = 0.5f;

    // Angular
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 angularVelocity = glm::vec3(0.0f);
    glm::vec3 accumTorque = glm::vec3(0.0f);
    glm::mat3 inertiaTensorLocal = glm::mat3(1.0f);
    glm::mat3 inverseInertiaWorld = glm::mat3(1.0f);

    // Damping
    float linearDamping = 0.99f;
    float angularDamping = 0.99f;

    Object* parent;
    bool hasGravity;

    Rigidbody(float mass, Object* parent, bool gravity);

    void SetMass(float tempMass);
    void ApplyForce(const glm::vec3& force);
    void ApplyTorque(const glm::vec3& torque);
    void ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point);
    void Integrate(float dt);

    glm::mat3 GetInertiaTensorWorld() const;
    void SetBoxInertiaTensor(const glm::vec3& halfExtents);

private:
    glm::mat3 EulerAnglesToRotationMatrix(const glm::vec3& eulerAngles) const;
};