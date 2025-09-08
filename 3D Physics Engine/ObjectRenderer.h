#pragma once
#include "VAO.h"
#include "glm/glm.hpp"
#include "shaderClass.h"
#include "Camera.h"
#include "Objects.h"
#include <glm/gtc/type_ptr.hpp>

class ObjectRenderer {
public:
    glm::mat4 projection;
    VBO* objectVBO;
    VAO* objectVAO;
    Shader* objectShader;
    unsigned int vertexCount;  // Store vertex count for proper drawing

    ObjectRenderer(ShapeType shape) {
        objectShader = new Shader("default.vert", "default.frag");

        float* verts = nullptr;
        vertexCount = 0;

        if (shape == ShapeType::Cube) {
            static float cubeVerts[] = {
                // Front face
                -0.5f, -0.5f,  0.5f,  // Bottom-left
                 0.5f, -0.5f,  0.5f,  // Bottom-right
                 0.5f,  0.5f,  0.5f,  // Top-right

                 0.5f,  0.5f,  0.5f,  // Top-right
                -0.5f,  0.5f,  0.5f,  // Top-left
                -0.5f, -0.5f,  0.5f,  // Bottom-left

                // Back face
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f,

                 0.5f,  0.5f, -0.5f,
                 0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,

                // Left face
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,

                -0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,

                // Right face
                 0.5f, -0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f,
                 0.5f,  0.5f,  0.5f,

                 0.5f,  0.5f,  0.5f,
                 0.5f, -0.5f,  0.5f,
                 0.5f, -0.5f, -0.5f,

                 // Top face
                 -0.5f,  0.5f, -0.5f,
                 -0.5f,  0.5f,  0.5f,
                  0.5f,  0.5f,  0.5f,

                  0.5f,  0.5f,  0.5f,
                  0.5f,  0.5f, -0.5f,
                 -0.5f,  0.5f, -0.5f,

                 // Bottom face
                 -0.5f, -0.5f, -0.5f,
                  0.5f, -0.5f, -0.5f,
                  0.5f, -0.5f,  0.5f,

                  0.5f, -0.5f,  0.5f,
                 -0.5f, -0.5f,  0.5f,
                 -0.5f, -0.5f, -0.5f
            };
            verts = cubeVerts;
            vertexCount = 36;
        }
        else if (shape == ShapeType::Plane) {
            static float planeVerts[] = {
                // Triangle 1
                -0.5f, 0.0f, -0.5f,  // bottom-left
                 0.5f, 0.0f, -0.5f,  // bottom-right
                 0.5f, 0.0f,  0.5f,  // top-right

                 // Triangle 2
                  0.5f, 0.0f,  0.5f,  // top-right
                 -0.5f, 0.0f,  0.5f,  // top-left
                 -0.5f, 0.0f, -0.5f   // bottom-left
            };
            verts = planeVerts;
            vertexCount = 6;  // 6 vertices for plane, not 36
        }

        objectVAO = new VAO();
        // Populate the VBO with the vertices
        objectVBO = new VBO(verts, vertexCount * 3 * sizeof(float));
        objectVAO->Bind();
        objectVBO->Bind();
        /*
        Attribute Index : The variable in the vertex shader that the data from the VBO will go
        Size : 3 values per index (x,y,z)
        Type : FLOAT
        Stride : The distance between vertices in bytes
        */
        objectVAO->LinkAttrib(*objectVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        objectVAO->Unbind();
        objectVBO->Unbind();
    }

    void SetProjection(glm::mat4 t_proj) {
        projection = t_proj;
    }

    void Draw(const glm::mat4& model, const glm::vec3& colour, Camera& camera) {
        objectShader->Activate();
        objectVAO->Bind();

        camera.Matrix(45.0f, 0.1f, 100.0f, *objectShader, "cameraMatrix");

        GLint modelLoc = glGetUniformLocation(objectShader->ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint colorLoc = glGetUniformLocation(objectShader->ID, "objectColor");
        glUniform3fv(colorLoc, 1, glm::value_ptr(colour));

        // Use the correct vertex count for drawing
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
};