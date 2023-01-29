//
//  Model.h
//  Starfighter
//
//  Created by Matt Montag on 11/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

/*
 A model class for representing 3D models, including vertices, materials, materials, faces.
 Loads .OBJ files.
 
 Model is to GameObject3D as Texture is to GameObject.
 GameObject3D should own a Model instance. 
 GameObject3D subclasses should construct their Model instance based on a filename.
 
 Model's "render" method will be called in the OpenGL context...?
 */

#ifndef Model_h
#define Model_h

#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Triangle {
    Vertex vertices[3];
//    glm::vec3 vertices[3];
//    glm::vec3 normals[3];
//    glm::vec3 colors[3];
};

class Model {
public:
    GLuint vbo;
    GLuint vao;
    vector<Triangle> triangles;

    Model(const char* filename) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;
        string err;

        GPU_LogInfo("Loading %s...\n", filename);
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
        if (!err.empty()) {
            GPU_LogError("%s\n", err.c_str());
        }
        if (!ret) {
            GPU_LogError("Obj file load failed for %s\n", filename);
            return;
        }
        GPU_LogInfo("...# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
        GPU_LogInfo("...# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
        GPU_LogInfo("...# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
        GPU_LogInfo("...# of materials = %d\n", (int)materials.size());
        GPU_LogInfo("...# of shapes    = %d\n", (int)shapes.size());

        for (int shape = 0; shape < shapes.size(); shape++) {
            for (int face = 0; face < shapes[shape].mesh.indices.size() / 3; face++) {
                tinyobj::index_t idx0 = shapes[shape].mesh.indices[3 * face + 0];
                tinyobj::index_t idx1 = shapes[shape].mesh.indices[3 * face + 1];
                tinyobj::index_t idx2 = shapes[shape].mesh.indices[3 * face + 2];

                int current_material_id = shapes[shape].mesh.material_ids[face];
                glm::vec3 color = glm::make_vec3(materials[current_material_id].diffuse);

                glm::vec3 vertices[3];
                for (int k = 0; k < 3; k++) {
                    int f0 = idx0.vertex_index;
                    int f1 = idx1.vertex_index;
                    int f2 = idx2.vertex_index;
                    vertices[0][k] = attrib.vertices[3 * f0 + k];
                    vertices[1][k] = attrib.vertices[3 * f1 + k];
                    vertices[2][k] = attrib.vertices[3 * f2 + k];
// Update mesh bounding box
//                    bmin[k] = std::min(v[0][k], bmin[k]);
//                    bmin[k] = std::min(v[1][k], bmin[k]);
//                    bmin[k] = std::min(v[2][k], bmin[k]);
//                    bmax[k] = std::max(v[0][k], bmax[k]);
//                    bmax[k] = std::max(v[1][k], bmax[k]);
//                    bmax[k] = std::max(v[2][k], bmax[k]);
                }

                glm::vec3 normals[3];
                if (attrib.normals.size() > 0) {
                    int f0 = idx0.normal_index;
                    int f1 = idx1.normal_index;
                    int f2 = idx2.normal_index;
                    for (int k = 0; k < 3; k++) {
                        normals[0][k] = attrib.normals[3 * f0 + k];
                        normals[1][k] = attrib.normals[3 * f1 + k];
                        normals[2][k] = attrib.normals[3 * f2 + k];
                    }
                } else {
                    // Compute normals
                    glm::vec3 edge1 = vertices[2] - vertices[0];
                    glm::vec3 edge2 = vertices[1] - vertices[0];
                    glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));
                    normals[0] = normals[1] = normals[2] = normal;
                }

                Triangle t;
                for (int i = 0; i < 3; i++) {
                    Vertex v;
                    v.position = vertices[i];
                    v.normal = normals[i];
                    v.color = color;
                    t.vertices[i] = v;
                }
                triangles.push_back(t);
            }
            
        }

        if (triangles.size() > 0) {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // We can pass a pointer to the struct packed inside the vector.
            // See https://herbsutter.com/2008/04/07/cringe-not-vectors-are-guaranteed-to-be-contiguous/
            glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), &(triangles[0]), GL_STATIC_DRAW);
            GPU_LogInfo("...# of triangles = %lu\n", triangles.size());
        } else {
            GPU_LogError("No triangles in obj file %s\n", filename);
        }
    }

    void render(GLuint vertex_loc, GLuint normal_loc, GLuint color_loc) {
        GLsizei stride = 36;

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(vertex_loc);
        glEnableVertexAttribArray(normal_loc);
        glEnableVertexAttribArray(color_loc);

        static long vec3size = sizeof(glm::vec3);
        //                     handle    size  type   normalize  stride         offset
        glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, stride, (void*)(vec3size*0));
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, stride, (void*)(vec3size*1));
        glVertexAttribPointer(color_loc,  3, GL_FLOAT, GL_FALSE, stride, (void*)(vec3size*2));

        glDrawArrays(GL_TRIANGLES, 0, 3 * (int)triangles.size());

        glDisableVertexAttribArray(color_loc);
        glDisableVertexAttribArray(normal_loc);
        glDisableVertexAttribArray(vertex_loc);

    }

};

#endif /* Model_h */
