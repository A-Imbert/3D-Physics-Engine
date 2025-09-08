#include "Collider.h"
#include "Object.h"

Collider::Collider(Object* t_parent) : parent(t_parent) {
    if (parent) {
        parent->setCollider(this);
    }
}