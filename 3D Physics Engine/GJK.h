#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cfloat>
#include "Collider.h"
#include "CollisionPoints.h"
#include "SupportPoint.h"

class GJK {
public:

    int iterations = 64;

    glm::vec3 Support(const Collider* a, const Collider* b, const glm::vec3& direction) {
        glm::vec3 pointA = a->FarthestPointInDirection(direction);
        glm::vec3 pointB = b->FarthestPointInDirection(-direction);
        return { pointA - pointB};
    }

    bool RunGJK(const Collider* colliderA, const Collider* colliderB, glm::vec3* mtv = nullptr) {
        //Simplex points (a is the first --> important for running functions again if it fails)
        glm::vec3 a(0,0,0), b(0, 0, 0), c(0, 0, 0), d(0, 0, 0);
        glm::vec3 searchDirection = { glm::normalize(colliderB->parent->transform->position - colliderA->parent->transform->position) };
        //First Support point
        c = Support(colliderA, colliderB, searchDirection);
        //For the next direction search away from the point that was just found
        searchDirection = -c;
        
        b = Support(colliderA, colliderB, searchDirection);

        //if the point does not pass the origin, then it will never enclose it
        if (!SameDirection(b, searchDirection)) return false;

        glm::vec3 BC = c - b;
        glm::vec3 BO = -b;
        searchDirection = glm::cross(glm::cross(BC, BO), BC);
        if (searchDirection == glm::vec3(0, 0, 0)) {
            searchDirection = glm::cross(BC, glm::vec3(1, 0, 0));
            if (searchDirection == glm::vec3(0, 0, 0)) {
                searchDirection = glm::cross(BC, glm::vec3(0, 0, -1));
            }
        }
        int simplexDimension = 2;
        for (int i = 0; i < iterations; i++) {
            a = Support(colliderA, colliderB, searchDirection);
            if (!SameDirection(a, searchDirection)) return false;
            simplexDimension++;
            if (simplexDimension == 3) {
                Triangle(a, b, c, d, simplexDimension, searchDirection);
            }
            else if (Tetrahedron(a, b, c, d, simplexDimension, searchDirection)) {
                if (mtv) *mtv = RunEPA(a, b, c, d, colliderA, colliderB);
                return true;
            }
        }
        return false;
    }

    //bool NextSimplex(std::vector<SupportPoint>& simplex, glm::vec3& direction) {
    //    switch (simplex.size()) {
    //        case 2: return Line(simplex, direction);
    //        case 3: return Triangle(simplex, direction);
    //        case 4: return Tetrahedron(simplex, direction);
    //    }
    //    return false;
    //}
    //Check if the region contains the origin
    bool SameDirection(const glm::vec3& direction, const glm::vec3& ao) {
        return glm::dot(direction, ao) > 1e-6f;
    }

    bool Line(std::vector<SupportPoint>& simplex, glm::vec3& direction) {
        glm::vec3 a = simplex[1].difference;
        glm::vec3 b = simplex[0].difference;

        glm::vec3 AB = b - a;
        glm::vec3 AO = -a;

        //Old method
        //float t = -glm::dot(a, AB) / glm::dot(AB, AB);

        //if (t <= 0) {
        //    direction = -a;
        //    simplex = { simplex[1] };
        //}
        //else if (t >= 1) {
        //    direction = -b;
        //    simplex = { simplex[0] };
        //}
        //else {
        //    glm::vec3 closestPoint = a + t * AB;
        //    direction = -closestPoint;
        //}
        //return false;
        if (SameDirection(AB, AO)) {
            direction = glm::cross(glm::cross(AB, AO), AB);
        }
        else {
            simplex = { simplex[1] };
            direction = AO;
        }
        return false;
    }

    void Triangle(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d, int& simplexDimension, glm::vec3& searchDirection) {
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ao = -a;
        //Calculate the triangles normal
        glm::vec3 normal = glm::cross(ab, ac);
        
        //Check if origin is in the Voronoi Region on the AB edge
        if (SameDirection(glm::cross(ab, normal), ao)) {
            c = a;
            searchDirection = glm::cross(glm::cross(ab, ao), ab);
            simplexDimension = 2;
            return;
        }
        //Check if origin is in the Voronoi Region on the AC edge
        if (SameDirection(glm::cross(ac, normal), ao)) {
            simplexDimension = 2;
            searchDirection = glm::cross(glm::cross(ac, ao), ac);
            b = a;
        }

        //If it's not on the AB or AC edge it must be above or below the triangle
        simplexDimension = 3;
        
        if (SameDirection(normal, ao)) {
            //Prepare simplex for Tetrahedron
            d = c;
            c = b;
            b = a;
            searchDirection = normal;
            return;
        }
        else {
            d = b;
            b = a;
            searchDirection = -normal;
            return;
        }
        
        //glm::vec3 a = simplex[2].difference;
        //glm::vec3 b = simplex[1].difference;
        //glm::vec3 c = simplex[0].difference;

        //glm::vec3 ab = b - a;
        //glm::vec3 ac = c - a;
        //glm::vec3 ao = -a;

        ////Calculate the triangles normal direction
        //glm::vec3 abc = glm::cross(ab, ac);
        ////Check if the origin is outside the triangle on the AC face
        //if (SameDirection(glm::cross(abc, ac), ao)) {
        //    //If its also in the direction of ac
        //    if (SameDirection(ac, ao)) {
        //        //remove b from the simplex
        //        simplex = { simplex[2], simplex[0]};
        //        //search perpendicular to AC towards origin
        //        direction = glm::cross(glm::cross(ac, ao), ac);
        //    }
        //    else {
        //        simplex = { simplex[2], simplex[1]};
        //        return Line( simplex, direction);
        //    }
        //}
        ////Check if the origin is in the AB edge region
        //else if (SameDirection(glm::cross(ab, abc), ao)) {
        //    simplex = { simplex[2], simplex[1] };
        //    return Line(simplex, direction);
        //}
        //else {
        //    //If both have failed that means the origin is above or below the triangle
        //        
        //    //Search in direction of normal
        //    if (SameDirection(abc, ao)) {
        //        direction = abc;
        //    }
        //    else {
        //        direction = -abc;
        //    }
        //}
        //return false;
    }

    bool Tetrahedron(glm::vec3 &a, glm::vec3& b, glm::vec3& c, glm::vec3& d, int& simplexDimension, glm::vec3& searchDirection){
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ad = d - a;
        glm::vec3 ao = -a;

        glm::vec3 abc = glm::cross(ab, ac);
        glm::vec3 acd = glm::cross(ac, ad);
        glm::vec3 adb = glm::cross(ad, ab);
        simplexDimension = 3;
        if (SameDirection(abc, ao)) {
            d = c;
            c = b;
            b = a;
            searchDirection = abc;
            return false;
        }
        if (SameDirection(acd, ao)) {
            b = a;
            searchDirection = acd;
            return false;
        }
        if (SameDirection(adb, ao)) {
            c = d;
            d = b;
            b = a;
            searchDirection = adb;
            return false;
        }
        return true;
    }
    #define EPA_TOLERANCE 0.0001
    #define EPA_MAX_NUM_FACES 64
    #define EPA_MAX_NUM_LOOSE_EDGES 32
    #define EPA_MAX_NUM_ITERATIONS 64
    glm::vec3 RunEPA(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
        const Collider* colliderA, const Collider* colliderB) {
        glm::vec3 faces[EPA_MAX_NUM_FACES][4]; //Array of faces, each with 3 verts and a normal
        // Init with final simplex from GJK
        faces[0][0] = a;
        faces[0][1] = b;
        faces[0][2] = c;
        faces[0][3] = glm::normalize(cross(b - a, c - a)); //ABC
        faces[1][0] = a;
        faces[1][1] = c;
        faces[1][2] = d;
        faces[1][3] = glm::normalize(cross(c - a, d - a)); //ACD
        faces[2][0] = a;
        faces[2][1] = d;
        faces[2][2] = b;
        faces[2][3] = glm::normalize(cross(d - a, b - a)); //ADB
        faces[3][0] = b;
        faces[3][1] = d;
        faces[3][2] = c;
        faces[3][3] = glm::normalize(cross(d - b, c - b)); //BDC

        int num_faces = 4;
        int closest_face;
        for (int iterations = 0; iterations < EPA_MAX_NUM_ITERATIONS; iterations++) {
            //Find face that's closest to origin
            float minDistance = glm::dot(faces[0][0], faces[0][3]);
            closest_face = 0;
            for (int i = 1; i < num_faces; i++) {
                float dist = glm::dot(faces[i][0], faces[i][3]);
                if (dist < minDistance) {
                    minDistance = dist;
                    closest_face = i;
                }
            }

            //search normal to face that's closest to origin
            glm::vec3 searchDirection = faces[closest_face][3];
            glm::vec3 p = Support(colliderA, colliderB, searchDirection);

            if (glm::dot(p, searchDirection) - minDistance < EPA_TOLERANCE) {
                //Convergence (new point is not significantly further from origin)
                return faces[closest_face][3] * dot(p, searchDirection); //dot vertex with normal to resolve collision along normal!
            }

            glm::vec3 loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2]; //keep track of edges we need to fix after removing faces
            int num_loose_edges = 0;

            //Find all triangles that are facing p
            for (int i = 0; i < num_faces; i++)
            {
                if (dot(faces[i][3], p - faces[i][0]) > 0) //triangle i faces p, remove it
                {
                    //Add removed triangle's edges to loose edge list.
                    //If it's already there, remove it (both triangles it belonged to are gone)
                    for (int j = 0; j < 3; j++) //Three edges per face
                    {
                        glm::vec3 current_edge[2] = { faces[i][j], faces[i][(j + 1) % 3] };
                        bool found_edge = false;
                        for (int k = 0; k < num_loose_edges; k++) //Check if current edge is already in list
                        {
                            if (loose_edges[k][1] == current_edge[0] && loose_edges[k][0] == current_edge[1]) {
                                //Edge is already in the list, remove it
                                //THIS ASSUMES EDGE CAN ONLY BE SHARED BY 2 TRIANGLES (which should be true)
                                //THIS ALSO ASSUMES SHARED EDGE WILL BE REVERSED IN THE TRIANGLES (which 
                                //should be true provided every triangle is wound CCW)
                                loose_edges[k][0] = loose_edges[num_loose_edges - 1][0]; //Overwrite current edge
                                loose_edges[k][1] = loose_edges[num_loose_edges - 1][1]; //with last edge in list
                                num_loose_edges--;
                                found_edge = true;
                                k = num_loose_edges; //exit loop because edge can only be shared once
                            }
                        }//endfor loose_edges

                        if (!found_edge) { //add current edge to list
                            // assert(num_loose_edges<EPA_MAX_NUM_LOOSE_EDGES);
                            if (num_loose_edges >= EPA_MAX_NUM_LOOSE_EDGES) break;
                            loose_edges[num_loose_edges][0] = current_edge[0];
                            loose_edges[num_loose_edges][1] = current_edge[1];
                            num_loose_edges++;
                        }
                    }

                    //Remove triangle i from list
                    faces[i][0] = faces[num_faces - 1][0];
                    faces[i][1] = faces[num_faces - 1][1];
                    faces[i][2] = faces[num_faces - 1][2];
                    faces[i][3] = faces[num_faces - 1][3];
                    num_faces--;
                    i--;
                }//endif p can see triangle i
            }//endfor num_faces

            //Reconstruct polytope with p added
            for (int i = 0; i < num_loose_edges; i++)
            {
                // assert(num_faces<EPA_MAX_NUM_FACES);
                if (num_faces >= EPA_MAX_NUM_FACES) break;
                faces[num_faces][0] = loose_edges[i][0];
                faces[num_faces][1] = loose_edges[i][1];
                faces[num_faces][2] = p;
                faces[num_faces][3] = glm::normalize(cross(loose_edges[i][0] - loose_edges[i][1], loose_edges[i][0] - p));

                //Check for wrong normal to maintain CCW winding
                float bias = 0.000001; //in case dot result is only slightly < 0 (because origin is on face)
                if (dot(faces[num_faces][0], faces[num_faces][3]) + bias < 0) {
                    glm::vec3 temp = faces[num_faces][0];
                    faces[num_faces][0] = faces[num_faces][1];
                    faces[num_faces][1] = temp;
                    faces[num_faces][3] = -faces[num_faces][3];
                }
                num_faces++;
            }
        } //End for iterations
        printf("EPA did not converge\n");
        //Return most recent closest point
        return faces[closest_face][3] * dot(faces[closest_face][0], faces[closest_face][3]);
    }
    /*CollisionPoints RunEPA(const std::vector<SupportPoint>& simplex,
        const Collider* A, const Collider* B) {

        std::vector<SupportPoint> polytope(simplex.begin(), simplex.end());

        std::vector<size_t> faces = 
        { 
            0, 1, 2, 
            0, 3, 1, 
            0, 2, 3, 
            1, 3, 2 
        };
        
        std::pair<std::vector<glm::vec4>, size_t> normalsFaces = GetFaceNormals(polytope, faces);

        std::vector<glm::vec4>& normals = normalsFaces.first;
        size_t minFace = normalsFaces.second;
        glm::vec3 minNormal;
        float minDistance = FLT_MAX;
        while (minDistance == FLT_MAX) {
            minNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
            minDistance = normals[minFace].w;
            glm::vec3 support = Support(A, B, minNormal);
            float sDistance = glm::dot(minNormal, support.difference);
            if (abs(sDistance - minDistance) > 0.001f) {
                minDistance = FLT_MAX;
                std::vector<std::pair<size_t, size_t>> uniqueEdges;
                for (size_t i = 0; i < normals.size(); i++) {
                    if (SameDirection(normals[i], support.difference)) {
                        size_t f = i * 3;
                        AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
                        AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                        AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);
                        faces[f + 2] = faces.back();
                        faces.pop_back();
                        faces[f + 1] = faces.back();
                        faces.pop_back();
                        faces[f] = faces.back();
                        faces.pop_back();
                        normals[i] = normals.back();
                        normals.pop_back();
                        i--;
                    }
                }
                std::vector<size_t> newFaces;
                for (const std::pair<int, int>& edge : uniqueEdges) {
                    int edgeIndex1 = edge.first;
                    int edgeIndex2 = edge.second;
                    newFaces.push_back(edgeIndex1);
                    newFaces.push_back(edgeIndex2);
                    newFaces.push_back(polytope.size());
                }
                polytope.push_back(support);
                std::pair<std::vector<glm::vec4>, size_t> newNormalsFaces =
                    GetFaceNormals(polytope, newFaces);
                std::vector<glm::vec4>& newNormals = newNormalsFaces.first;
                size_t newMinFace = newNormalsFaces.second;
                float oldMinDistance = FLT_MAX;
                for (size_t i = 0; i < normals.size(); i++) {
                    if (normals[i].w < oldMinDistance) {
                        oldMinDistance = normals[i].w;
                        minFace = i;
                    }
                }
                if (newNormals[newMinFace].w < oldMinDistance) {
                    minFace = newMinFace + normals.size();
                }
                faces.insert(faces.end(), newFaces.begin(), newFaces.end());
                normals.insert(normals.end(), newNormals.begin(), newNormals.end());
            }
        }
        CollisionPoints points;
        points.normal = minNormal;
        points.penetrationDepth = minDistance + 0.001f;
        points.hasCollision = true;
        return points;
    }*/


private:
    std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
        const std::vector<SupportPoint>& polytope,
        const std::vector<size_t>& faces) {

        std::vector<glm::vec4> normals;
        size_t minTriangle = 0;
        float minDistance = FLT_MAX;

        for (size_t i = 0; i < faces.size(); i += 3) {
            glm::vec3 a = polytope[faces[i]].difference;
            glm::vec3 b = polytope[faces[i + 1]].difference;
            glm::vec3 c = polytope[faces[i + 2]].difference;

            glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
            float distance = glm::dot(normal, a);

            if (distance < 0) {
                normal = -normal;
                distance = -distance;
            }

            normals.emplace_back(normal, distance);

            if (distance < minDistance) {
                minTriangle = i / 3;
                minDistance = distance;
            }
        }

        return { normals, minTriangle };
    }

    void AddIfUniqueEdge(
        std::vector<std::pair<size_t, size_t>>& edges,
        const std::vector<size_t>& faces,
        size_t a, size_t b) {

        auto reverse = std::find(
            edges.begin(),
            edges.end(),
            std::make_pair(faces[b], faces[a])
        );

        if (reverse != edges.end()) {
            edges.erase(reverse);
        }
        else {
            edges.emplace_back(faces[a], faces[b]);
        }
    }
};