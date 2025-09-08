#include "Rigidbody.h"
#include "Object.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

Rigidbody::Rigidbody(float mass, Object* parent, bool gravity) {
    this->parent = parent;
    hasGravity = gravity;
    SetMass(mass);
}

void Rigidbody::SetMass(float tempMass) {
    mass = tempMass;
    inverseMass = (mass == 0.0f) ? 0.0f : 1.0f / mass;
    SetBoxInertiaTensor(glm::vec3(1.0f, 1.0f, 1.0f));
}

void Rigidbody::ApplyForce(const glm::vec3& force) {
    accumForce += force;
}

void Rigidbody::ApplyTorque(const glm::vec3& torque) {
    accumTorque += torque;
}

void Rigidbody::ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
    accumForce += force;
    glm::vec3 r = point - parent->transform->position;
    accumTorque += glm::cross(r, force);
}

glm::mat3 Rigidbody::GetInertiaTensorWorld() const {
    glm::mat3 R = glm::mat3_cast(orientation);
    return R * inertiaTensorLocal * glm::transpose(R);
}

void Rigidbody::SetBoxInertiaTensor(const glm::vec3& halfExtents) {
    float x2 = 4.0f * halfExtents.x * halfExtents.x;
    float y2 = 4.0f * halfExtents.y * halfExtents.y;
    float z2 = 4.0f * halfExtents.z * halfExtents.z;

    float ix = (1.0f / 12.0f) * mass * (y2 + z2);
    float iy = (1.0f / 12.0f) * mass * (x2 + z2);
    float iz = (1.0f / 12.0f) * mass * (x2 + y2);

    inertiaTensorLocal = glm::mat3(
        glm::vec3(ix, 0.0f, 0.0f),
        glm::vec3(0.0f, iy, 0.0f),
        glm::vec3(0.0f, 0.0f, iz)
    );
}

glm::mat3 Rigidbody::EulerAnglesToRotationMatrix(const glm::vec3& eulerAngles) const {
    float cosX = cos(eulerAngles.x);
    float sinX = sin(eulerAngles.x);
    float cosY = cos(eulerAngles.y);
    float sinY = sin(eulerAngles.y);
    float cosZ = cos(eulerAngles.z);
    float sinZ = sin(eulerAngles.z);

    glm::mat3 rotationX = glm::mat3(
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, cosX, -sinX),
        glm::vec3(0.0f, sinX, cosX)
    );

    glm::mat3 rotationY = glm::mat3(
        glm::vec3(cosY, 0.0f, sinY),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-sinY, 0.0f, cosY)
    );

    glm::mat3 rotationZ = glm::mat3(
        glm::vec3(cosZ, -sinZ, 0.0f),
        glm::vec3(sinZ, cosZ, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    return rotationZ * rotationY * rotationX;
}

void Rigidbody::Integrate(float dt) {
    if (inverseMass == 0.0f) return;

    // Linear
    glm::vec3 acceleration = accumForce * inverseMass;
    velocity += acceleration * dt;
    velocity *= powf(linearDamping, dt);
    parent->transform->position += velocity * dt;

    // Angular
    inverseInertiaWorld = glm::inverse(GetInertiaTensorWorld());
    glm::vec3 angularAcceleration = inverseInertiaWorld * accumTorque;
    angularVelocity += angularAcceleration * dt;
    angularVelocity *= powf(angularDamping, dt);

    glm::quat spin = glm::quat(0.0f, angularVelocity) * orientation * 0.5f;
    orientation += spin * dt;
    orientation = glm::normalize(orientation);

    parent->transform->rotation = glm::degrees(glm::eulerAngles(orientation));

    // Clear accumulators
    accumForce = glm::vec3(0.0f);
    accumTorque = glm::vec3(0.0f);
}