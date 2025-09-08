#pragma once
#include <glm/glm.hpp>

class Object;
class AABB;

class Collider {
public:
    Object* parent;

    Collider(Object* t_parent);

    virtual void Update() = 0;
    virtual glm::vec3 FarthestPointInDirection(const glm::vec3& direction) const = 0;
    virtual const AABB& GetAABB() const = 0;
    virtual glm::vec3 GetCenter() const = 0;
};