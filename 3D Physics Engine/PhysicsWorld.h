#pragma once
#include <vector>
#include "Object.h"
#include "Broadphase.h"
#include "BoxCollider.h"
#include "CollisionPoints.h"
#include "GJK.h"
#include <set>
class PhysicsWorld {
public:
    glm::vec3 gravity;
    std::vector<Object*> objects;
    Broadphase* BP;

    PhysicsWorld(const glm::vec3& gravity = glm::vec3(0.0f, -9.81f, 0.0f))
        : gravity(gravity) {
    }

    void AddObject(Object* obj) {
        objects.push_back(obj);
    }

    void Step(float deltaTime) {
        for (auto obj : objects) {
            if (obj->collider == nullptr || obj->rb == nullptr) {
                continue;
            }
            // Apply gravity to each Rigidbody
            if (obj->rb->hasGravity) {
                obj->rb->ApplyForce(gravity * obj->rb->mass);
            }
            // Integrate motion
            obj->Update(deltaTime);
        }
        HandleCollisions();
    }

    void HandleCollisions() {
        std::vector<std::pair<Collider*, Collider*>> potentialPairs = BP->ComputePotentialPairs(objects);
        GJK gjk;

        // Track processed pairs to prevent double-processing
        std::set<std::pair<Collider*, Collider*>> processedPairs;

        for (auto& pair : potentialPairs) {
            Collider* a = pair.first;
            Collider* b = pair.second;

            // Ensure consistent ordering to avoid processing (A,B) and (B,A)
            if (a > b) std::swap(a, b);

            // Skip if already processed
            if (processedPairs.find({ a, b }) != processedPairs.end()) {
                std::cout << "Skipping duplicate pair!" << std::endl;
                continue;
            }
            processedPairs.insert({ a, b });

            std::vector<SupportPoint> simplex;
            glm::vec3 mtv;
            std::cout << "False Detection";
            if (gjk.RunGJK(a, b, &mtv)) {

                std::cout << "Collision detected between " << a << " and " << b << std::endl;
                std::cout << "mtv: " << mtv.x << " " << mtv.y << " " << mtv.z << " ";
                ResolveCollision(a, b, mtv);
            }
        }
    }

    void ResolveCollision(Collider* colliderA, Collider* colliderB, glm::vec3 mtv) {
        // MTV points from A to B to separate them
        float totalInverseMass = 0.0f;
        if (colliderA->parent->rb != nullptr) totalInverseMass += 1.0f / colliderA->parent->rb->mass;
        if (colliderB->parent->rb != nullptr) totalInverseMass += 1.0f / colliderB->parent->rb->mass;

        if (totalInverseMass == 0.0f) return; // Both static

        if (colliderA->parent->rb != nullptr) {
            float massRatio = (1.0f / colliderA->parent->rb->mass) / totalInverseMass;
            colliderA->parent->transform->position -= mtv * massRatio; // Move A away from B
        }
        if (colliderB->parent->rb != nullptr) {
            float massRatio = (1.0f / colliderB->parent->rb->mass) / totalInverseMass;
            colliderB->parent->transform->position += mtv * massRatio; // Move B away from A
        }
    }

};