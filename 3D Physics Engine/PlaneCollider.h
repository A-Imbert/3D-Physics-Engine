#pragma once
#include "Collider.h"
#include "AABB.h"
#include "Object.h"

class PlaneCollider : public Collider {
public:
    glm::vec3 offset;
    glm::vec2 baseHalfSize;  // Size of the plane (X, Z)
    glm::vec2 halfSize;      // Scaled size
    AABB aabb;

    PlaneCollider(Object* parentObject, glm::vec2 baseHalfSize = glm::vec2(0.5f), glm::vec3 offset = glm::vec3(0.0f))
        : Collider(parentObject), baseHalfSize(baseHalfSize), offset(offset) {
        UpdateHalfSize();
    }

    void Update() override {
        UpdateHalfSize();

        glm::vec3 center = parent->transform->position + offset;
        glm::mat3 rotation = EulerAnglesToRotationMatrix(parent->transform->rotation);

        // Local quad corners (flat in XZ plane)
        glm::vec3 localCorners[4] = {
            { -halfSize.x, 0.0f, -halfSize.y },
            {  halfSize.x, 0.0f, -halfSize.y },
            { -halfSize.x, 0.0f,  halfSize.y },
            {  halfSize.x, 0.0f,  halfSize.y }
        };

        // Compute rotated corners to find world-space AABB
        glm::vec3 minP(FLT_MAX), maxP(-FLT_MAX);
        for (int i = 0; i < 4; i++) {
            glm::vec3 worldCorner = center + rotation * localCorners[i];
            minP = glm::min(minP, worldCorner);
            maxP = glm::max(maxP, worldCorner);
        }

        aabb.min = minP;
        aabb.max = maxP;
    }

    glm::vec3 FarthestPointInDirection(const glm::vec3& direction) const override {
        glm::vec3 center = parent->transform->position + offset;
        glm::mat3 rotation = EulerAnglesToRotationMatrix(parent->transform->rotation);

        glm::vec3 localCorners[4] = {
            { -halfSize.x, 0.0f, -halfSize.y },
            {  halfSize.x, 0.0f, -halfSize.y },
            { -halfSize.x, 0.0f,  halfSize.y },
            {  halfSize.x, 0.0f,  halfSize.y }
        };

        glm::vec3 bestPoint;
        float maxDot = -FLT_MAX;
        for (int i = 0; i < 4; i++) {
            glm::vec3 worldCorner = center + rotation * localCorners[i];
            float d = glm::dot(worldCorner, direction);
            if (d > maxDot) {
                maxDot = d;
                bestPoint = worldCorner;
            }
        }
        return bestPoint;
    }

    const AABB& GetAABB() const override { return aabb; }

private:
    void UpdateHalfSize() {
        // XZ scaling, Y is flat
        halfSize = baseHalfSize * glm::vec2(parent->transform->scale.x, parent->transform->scale.z);
    }

    glm::mat3 EulerAnglesToRotationMatrix(const glm::vec3& eulerAngles) const {
        float cosX = cos(eulerAngles.x), sinX = sin(eulerAngles.x);
        float cosY = cos(eulerAngles.y), sinY = sin(eulerAngles.y);
        float cosZ = cos(eulerAngles.z), sinZ = sin(eulerAngles.z);

        glm::mat3 rotX = glm::mat3(
            glm::vec3(1, 0, 0),
            glm::vec3(0, cosX, -sinX),
            glm::vec3(0, sinX, cosX)
        );
        glm::mat3 rotY = glm::mat3(
            glm::vec3(cosY, 0, sinY),
            glm::vec3(0, 1, 0),
            glm::vec3(-sinY, 0, cosY)
        );
        glm::mat3 rotZ = glm::mat3(
            glm::vec3(cosZ, -sinZ, 0),
            glm::vec3(sinZ, cosZ, 0),
            glm::vec3(0, 0, 1)
        );
        return rotZ * rotY * rotX;
    }
};
