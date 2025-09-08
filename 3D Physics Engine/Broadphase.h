#pragma once
#include <vector>
#include <algorithm>
#include "Object.h"
#include "Collider.h"
#include "AABB.h"
class Object;
class Broadphase {
public:
    struct EndPoint {
        float value;
        bool isMin;
        Collider* owner;

        EndPoint(float tValue, bool tIsMin, Collider* tOwner) {
            value = tValue;
            isMin = tIsMin;
            owner = tOwner;
        }

        bool operator<(const EndPoint& other) const {
            return value < other.value || (value == other.value && isMin && !other.isMin);
        }
    };
    std::vector<std::pair<Collider*, Collider*>> ComputePotentialPairs(const std::vector<Object*>& objects) {
        // Build endpoint list
        std::vector<EndPoint> endpoints;
        for (Object* obj: objects) {
            if (!obj->collider) continue;
            obj->collider->Update(); // Update AABB based on object transform
            const AABB& box = obj->collider->GetAABB();
            endpoints.emplace_back(box.min.x, true, obj->collider);
            endpoints.emplace_back(box.max.x, false, obj->collider);
        }

        // Sort by x-axis value
        std::sort(endpoints.begin(), endpoints.end());

        // Sweep and collect overlapping pairs
        std::vector<std::pair<Collider*, Collider*>> potentialPairs;
        //The active list tracks the objects whose min we've seen but whose max we haven't
        std::vector<Collider*> active;

        for (const EndPoint& ep : endpoints) {
            if (ep.isMin) {
                //Loop through only ACTIVE colliders
                //If they aren't active it means theres no way they can be intersecting
                for (Collider* other : active) {
                    if (AABBOverlapOnYZ(ep.owner->GetAABB(), other->GetAABB())) {
                        potentialPairs.emplace_back(ep.owner, other);
                    }
                }
                //Add to the list of actives for other mins to check against
                active.push_back(ep.owner);
            }
            else {
                //If a max is detected it means that this box's projection is over
                active.erase(std::remove(active.begin(), active.end(), ep.owner), active.end());
            }
        }

        return potentialPairs;
    }

private:
    bool AABBOverlapOnYZ(const AABB& a, const AABB& b) {
        return (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
            (a.min.z <= b.max.z && a.max.z >= b.min.z);
    }
};