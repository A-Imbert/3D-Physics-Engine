#pragma once
#include "Collider.h"
#include "AABB.h"
#include "Object.h"

class BoxCollider : public Collider {
public:
    glm::vec3 offset;
    glm::vec3 baseHalfSize;  // The base size of the collision box
    glm::vec3 halfSize;
    AABB aabb;

 
    BoxCollider(Object* parentObject, glm::vec3 offset = glm::vec3(0.0f), glm::vec3 baseHalfSize = glm::vec3(0.5f, 0.5f, 0.5f))
        : Collider(parentObject), offset(offset), baseHalfSize(baseHalfSize) {
        UpdateHalfSize();
    }

    // Updates the collider when the object is moved or scaled
    void Update() {
        UpdateHalfSize();  // Recalculate in case scale changed

        glm::vec3 center = parent->transform->position + offset;
        glm::mat3 rotation = EulerAnglesToRotationMatrix(parent->transform->rotation);

        // Compute world AABB from rotated OBB
        glm::vec3 x = rotation[0] * halfSize.x;
        glm::vec3 y = rotation[1] * halfSize.y;
        glm::vec3 z = rotation[2] * halfSize.z;
        glm::vec3 extent = glm::abs(x) + glm::abs(y) + glm::abs(z);

        aabb.min = center - extent;
        aabb.max = center + extent;
    }

private:
    void UpdateHalfSize() {
        halfSize = baseHalfSize * parent->transform->scale;
    }

public:
    glm::mat3 EulerAnglesToRotationMatrix(glm::vec3& eulerAngles) const {
        float cosX = cos(eulerAngles.x);
        float sinX = sin(eulerAngles.x);
        float cosY = cos(eulerAngles.y);
        float sinY = sin(eulerAngles.y);
        float cosZ = cos(eulerAngles.z);
        float sinZ = sin(eulerAngles.z);

        glm::mat3 tranformationX = glm::mat3(
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, cosX, -sinX),
            glm::vec3(0.0f, sinX, cosX)
        );
        glm::mat3 tranformationY = glm::mat3(
            glm::vec3(cosY, 0.0f, sinY),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(-sinY, 0.0f, cosY)
        );
        glm::mat3 tranformationZ = glm::mat3(
            glm::vec3(cosZ, -sinZ, 0.0f),
            glm::vec3(sinZ, cosZ, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        return tranformationZ * tranformationY * tranformationX;
    }

    glm::vec3 FarthestPointInDirection(const glm::vec3& direction) const {
        glm::vec3 center = parent->transform->position + offset;
        glm::vec3 vertsInLocal[] = {
            { -1, -1, -1 }, { 1, -1, -1 }, { -1,  1, -1 }, { 1,  1, -1 },
            { -1, -1,  1 }, { 1, -1,  1 }, { -1,  1,  1 }, { 1,  1,  1 }
        };

        glm::mat3 rotation = EulerAnglesToRotationMatrix(parent->transform->rotation);
        glm::vec3 bestPoint;
        float maxDot = -FLT_MAX;

        for (int i = 0; i < 8; i++) {
            glm::vec3 localCorner = vertsInLocal[i] * halfSize;
            glm::vec3 worldCorner = center + rotation * localCorner;
            float d = glm::dot(worldCorner, direction);
            if (d > maxDot) {
                maxDot = d;
                bestPoint = worldCorner;
            }
        }
        return bestPoint;
    }
    glm::vec3 GetCenter() const override {
        return parent->transform->position + offset;
    }
    const AABB& GetAABB() const { return aabb; }
};