//
//  OpenGLRenderer.h
//  Starfighter
//
//  Created by Matt Montag on 11/14/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef OpenGLRenderer_h
#define OpenGLRenderer_h

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"

class OpenGLRenderer {
public:
    GLuint VBO;
    GLuint VAO;
    GLuint modelViewProjection_loc;
    GLuint model_loc;
    GLuint vertex_loc;
    GLuint normal_loc;
    GLuint color_loc;
    GLuint v, f, p;

    GLfloat quadVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    GLuint vaoQuad;
    GLuint vboQuad;
    GLuint screenShaderProgram;
    GLuint frameBuffer;
    GLuint texColorBuffer;
    GLint screenViewport[4];
    SDL_Rect gameBounds;

    Model* model;

    OpenGLRenderer(const SDL_Rect& gameBounds) {
        this->gameBounds = gameBounds;
        glGetIntegerv(GL_VIEWPORT, screenViewport);

        // glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK) {
            GPU_LogError("Initialization Error: Failed to initialize GLEW.\n");
        }

//=================================

        v = GPU_LoadVertexShader("shaders/untextured.vert");
        f = GPU_LoadFragmentShader("shaders/passthrough.frag");
        p = GPU_LinkShaders(v, f);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        vertex_loc = GPU_GetAttributeLocation(p, "gpu_Vertex");
        normal_loc = GPU_GetAttributeLocation(p, "gpu_Normal");
        color_loc = GPU_GetAttributeLocation(p, "gpu_Color");
        model_loc = GPU_GetUniformLocation(p, "gpu_ModelMatrix");
        modelViewProjection_loc = GPU_GetUniformLocation(p, "gpu_ModelViewProjectionMatrix");

//=================================

        // Create VAOs
        glGenVertexArrays(1, &vaoQuad);
        glBindVertexArray(vaoQuad);

        // Load vertex data
        glGenBuffers(1, &vboQuad);
        glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Create shader programs
        GLuint screenVertexShader = GPU_LoadVertexShader("shaders/screen.vert");
        GLuint screenFragmentShader = GPU_LoadFragmentShader("shaders/screen.frag");
        screenShaderProgram = GPU_LinkShaders(screenVertexShader, screenFragmentShader);

        // Specify the layout of the vertex data
        specifyScreenVertexAttributes(screenShaderProgram);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "texFramebuffer"), 0);

        // Create framebuffer
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        // Create texture to hold color buffer. No Renderbuffer object for depth/stencil buffer is necessary.
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameBounds.w, gameBounds.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        model = new Model("models/cube.obj");
    }

    GLuint GPU_LoadVertexShader(const char* filename) {
        GLuint s = GPU_LoadShader(GPU_VERTEX_SHADER, filename);
        if (!s) {
            GPU_LogError("Error loading vertex shader: %s", GPU_GetShaderMessage());
        }
        return s;
    }

    GLuint GPU_LoadFragmentShader(const char* filename) {
        GLuint s = GPU_LoadShader(GPU_FRAGMENT_SHADER, filename);
        if (!s) {
            GPU_LogError("Error loading fragment shader: %s", GPU_GetShaderMessage());
        }
        return s;
    }

    void specifyScreenVertexAttributes(GLuint shaderProgram)
    {
        GLint posAttrib = GPU_GetAttributeLocation(shaderProgram, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

        GLint texAttrib = GPU_GetAttributeLocation(shaderProgram, "texcoord");
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    }

    void begin_3d()
    {
        // Draw queued blit buffers
        GPU_FlushBlitBuffer();

        // Resize viewport to texture size
        glViewport(0, 0, gameBounds.w, gameBounds.h);

        // TODO: re-enable backface culling. just disabeld for developing the model loader.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Switch framebuffer destination to texture and clear
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void draw_3d_stuff() {
        begin_3d();

        // Camera Position
        glm::mat4 viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)gameBounds.w / gameBounds.h, 0.1f, 1000.0f);

        float t = SDL_GetTicks()/1000.0f;

        // Model transform - should come from GameObject
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::rotate(modelMat, float(glm::radians(50.0 * t)), glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat, float(glm::radians(50.0 * t)), glm::vec3(1, 0, 0));

//        // Mesh creation
//        int stride = 10;
//        const int numTriangles = 4;
//        GLfloat gldata[stride * numTriangles * 3];
//
//        struct Triangle {
//            glm::vec3 verts[3];
//            glm::vec4 color;
//            glm::vec3 normal;
//        };
//
//        Triangle triangles[numTriangles] = {
//            {
//                glm::vec3{ 1, 1, 1},
//                glm::vec3{ 1,-1,-1},
//                glm::vec3{-1, 1,-1},
//                glm::vec4{0.5, 0.5, 0.5, 1.0},
//            },
//            {
//                glm::vec3{ 1,-1,-1},
//                glm::vec3{-1,-1, 1},
//                glm::vec3{-1, 1,-1},
//                glm::vec4{0.5, 0.5, 0.5, 1.0},
//            },
//            {
//                glm::vec3{ 1, 1, 1},
//                glm::vec3{-1, 1,-1},
//                glm::vec3{-1,-1, 1},
//                glm::vec4{0.5, 0.5, 0.5, 1.0},
//            },
//            {
//                glm::vec3{ 1, 1, 1},
//                glm::vec3{-1,-1, 1},
//                glm::vec3{ 1,-1,-1},
//                glm::vec4{0.5, 0.5, 0.5, 1.0},
//            },
//        };
//        for (int tr = 0; tr < numTriangles; tr++) {
//            Triangle tri = triangles[tr];
//
//            // Normals
//            for (int i = 0; i < 3; i++) {
//                glm::vec3 edge1 = tri.verts[2] - tri.verts[0];
//                glm::vec3 edge2 = tri.verts[1] - tri.verts[0];
//                tri.normal = glm::normalize(glm::cross(edge2, edge1));
//            }
//
//            for (int i = 0; i < 3; i++) {
//                // Vertex
//                int offset = 0;
//                int vertIdx = tr * 3 + i;
//                gldata[vertIdx * stride + offset + 0] = tri.verts[i][0];
//                gldata[vertIdx * stride + offset + 1] = tri.verts[i][1];
//                gldata[vertIdx * stride + offset + 2] = tri.verts[i][2];
//                // Normals
//                offset += 3;
//                gldata[vertIdx * stride + offset + 0] = tri.normal[0];
//                gldata[vertIdx * stride + offset + 1] = tri.normal[1];
//                gldata[vertIdx * stride + offset + 2] = tri.normal[2];
//                // Colors
//                offset += 3;
//                gldata[vertIdx * stride + offset + 0] = tri.color[0];
//                gldata[vertIdx * stride + offset + 1] = tri.color[1];
//                gldata[vertIdx * stride + offset + 2] = tri.color[2];
//                gldata[vertIdx * stride + offset + 3] = tri.color[3];
//            }
//        }

        glUseProgram(p);
        // Update model/view/projection uniform matrices
        const float* m_ptr = glm::value_ptr(modelMat);
        const float* mvp_ptr = glm::value_ptr(projMat * viewMat * modelMat);
        glUniformMatrix4fv(model_loc, 1, 0, m_ptr);
        glUniformMatrix4fv(modelViewProjection_loc, 1, 0, mvp_ptr);

        this->model->render(vertex_loc, normal_loc, color_loc);

//        glBindVertexArray(VAO);
//
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(gldata), gldata, GL_STREAM_DRAW);
//
//        glEnableVertexAttribArray(vertex_loc);
//        glEnableVertexAttribArray(normal_loc);
//        glEnableVertexAttribArray(color_loc);
//
//        glVertexAttribPointer(vertex_loc,
//                              3,                    // size
//                              GL_FLOAT,             // type
//                              GL_FALSE,             // normalize
//                              sizeof(float)*stride, // stride
//                              (void*)0              // offset
//                              );
//
//        glVertexAttribPointer(normal_loc,
//                              3,                        // size
//                              GL_FLOAT,                 // type
//                              GL_FALSE,                 // normalize
//                              sizeof(float)*stride,     // stride
//                              (void*)(sizeof(float)*3)  // offset
//                              );
//
//        glVertexAttribPointer(color_loc,
//                              4,                        // size
//                              GL_FLOAT,                 // type
//                              GL_FALSE,                 // normalize
//                              sizeof(float)*stride,     // stride
//                              (void*)(sizeof(float)*6)  // offset
//                              );
//
//        glDrawArrays(GL_TRIANGLES, 0, 3 * numTriangles);
//
//        glDisableVertexAttribArray(color_loc);
//        glDisableVertexAttribArray(normal_loc);
//        glDisableVertexAttribArray(vertex_loc);

        end_3d();
    }

    void end_3d()
    {
        // Restore viewport to window size
        glViewport(0, 0, screenViewport[2], screenViewport[3]);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        // Restore framebuffer destination to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Draw framebuffer to full-screen quad
        glBindVertexArray(vaoQuad);
        glUseProgram(screenShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

#endif /* OpenGLRenderer_h */
