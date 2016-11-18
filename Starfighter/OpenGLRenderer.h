//
//  OpenGLRenderer.h
//  Starfighter
//
//  Created by Matt Montag on 11/14/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef OpenGLRenderer_h
#define OpenGLRenderer_h

class OpenGLRenderer {
public:
    GLuint VBO;
      GLuint VAO;
    GLuint modelViewProjection_loc;
    GLuint vertex_loc;
    GLuint color_loc;
    Uint32 v, f, p;

    // Quad vertices
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
    GLuint panelVertexShader, panelFragmentShader, panelShaderProgram;
    GLuint modelViewProjection_loc2;
    //GLint  uniTexOffset;
    GLuint frameBuffer;
    GLuint texColorBuffer;
    GLuint rboDepthStencil;
    GLint screenViewport[4];

    SDL_Rect gameBounds;

    OpenGLRenderer(const SDL_Rect& gameBounds) {
        this->gameBounds = gameBounds;
        glGetIntegerv(GL_VIEWPORT, screenViewport);

        glewExperimental = GL_TRUE;  // Force GLEW to get exported functions instead of checking via extension string
        if(glewInit() != GLEW_OK)
        {
            GPU_LogError("Initialization Error: Failed to initialize GLEW.\n");
        }

        v = GPU_LoadShader(GPU_VERTEX_SHADER, "shaders/untextured.vert");
        printShaderLog(v);
        if (!v) {
            printf("Error loading vertex shader!\n");
        }
        f = GPU_LoadShader(GPU_FRAGMENT_SHADER, "shaders/untextured.frag");
        printShaderLog(f);
        if (!f) {
            printf("Error loading fragment shader!\n");
        }
        p = GPU_LinkShaders(v, f);
        printProgramLog(p);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        vertex_loc = GPU_GetAttributeLocation(p, "gpu_Vertex");
        color_loc = GPU_GetAttributeLocation(p, "gpu_Color");
        modelViewProjection_loc = GPU_GetUniformLocation(p, "gpu_ModelViewProjectionMatrix");

//=================================
        // Create VAOs
        glGenVertexArrays(1, &vaoQuad);

        // Load vertex data
        glGenBuffers(1, &vboQuad);
        glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Create shader programs
        createShaderProgram(panelVertexSource, panelFragmentSource, panelVertexShader, panelFragmentShader, panelShaderProgram);

        modelViewProjection_loc2 = GPU_GetUniformLocation(panelShaderProgram, "gpu_modelViewProjectionMatrix");


        // Specify the layout of the vertex data
        glBindVertexArray(vaoQuad);
        glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
        specifyScreenVertexAttributes(panelShaderProgram);

//        glUseProgram(panelShaderProgram);
        glUniform1i(glGetUniformLocation(panelShaderProgram, "texFramebuffer"), 0);
//        GPU_GetModelViewProjection(mvp);
//        glUniformMatrix4fv(modelViewProjection_loc, 1, 0, mvp);

        //uniTexOffset = glGetUniformLocation(panelShaderProgram, "texOffset");

        // Create framebuffer
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // <<<<---------------------------------------------------------------------------------------------------

        // Create texture to hold color buffer
//        GPU_SetWindowResolution(400, 300);
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameBounds.w, gameBounds.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        // Create Renderbuffer Object to hold depth and stencil buffers
        glGenRenderbuffers(1, &rboDepthStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gameBounds.w, gameBounds.h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
    }

    void printProgramLog( GLuint program )
    {
        //Make sure name is program
        if( glIsProgram( program ) )
        {
            //Program log length
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            //Get info string length
            glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

            //Allocate string
            char* infoLog = new char[ maxLength ];

            //Get info log
            glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
            if( infoLogLength > 0 )
            {
                //Print Log
                printf( "%s\n", infoLog );
            }

            //Deallocate string
            delete[] infoLog;
        }
        else
        {
            printf( "Name %d is not a program\n", program );
        }
    }

    void printShaderLog( GLuint shader )
    {
        //Make sure name is shader
        if( glIsShader( shader ) )
        {
            //Shader log length
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            //Get info string length
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

            //Allocate string
            char* infoLog = new char[ maxLength ];

            //Get info log
            glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
            if( infoLogLength > 0 )
            {
                //Print Log
                printf( "%s\n", infoLog );
            }
            
            //Deallocate string
            delete[] infoLog;
        }
        else
        {
            printf( "Name %d is not a shader\n", shader );
        }
    }

    const GLchar* panelVertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "in vec2 texcoord;"
    "out vec2 Texcoord;"
    "uniform mat4 gpu_ModelViewProjectionMatrix;"
    "void main()"
    "{"
    "    Texcoord = texcoord;"
    "    gl_Position = gpu_ModelViewProjectionMatrix * vec4(position, 0.0, 1.0);"
    "}";
    const GLchar* panelFragmentSource =
    "#version 150 core\n"
    "in vec2 Texcoord;"
    "out vec4 outColor;"
    "uniform sampler2D texFramebuffer;"
    "void main()"
    "{"
    "    outColor = texture(texFramebuffer, Texcoord);"
    "}";
    const GLchar* screenVertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "in vec2 texcoord;"
    "out vec2 Texcoord;"
    "void main()"
    "{"
    "    Texcoord = texcoord;"
    "    gl_Position = vec4(position, 0.0, 1.0);"
    "}";
    const GLchar* screenFragmentSource =
    "#version 150 core\n"
    "in vec2 Texcoord;"
    "out vec4 outColor;"
    "uniform sampler2D texFramebuffer;"
    "uniform vec2 texOffset;"
    "const float kernel[9] = float[]("
    "    0.00481007202f,"
    "    0.0286864862f,"
    "    0.102712765f,"
    "    0.220796734f,"
    "    0.284958780f,"
    "    0.220796734f,"
    "    0.102712765f,"
    "    0.0286864862f,"
    "    0.00481007202f"
    ");"
    "void main()"
    "{"
    "    vec4 sum = vec4(0.0);"
    "    for (int i = -4; i <= 4; i++)"
    "        sum += texture("
    "            texFramebuffer,"
    "            Texcoord + i * texOffset"
    "        ) * kernel[i + 4];"
    "    outColor = sum;"
    "}";

    void createShaderProgram(const GLchar* vertSrc, const GLchar* fragSrc, GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram)
    {
        // Create and compile the vertex shader
//        vertexShader = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vertexShader, 1, &vertSrc, NULL);
//        glCompileShader(vertexShader);
        vertexShader = GPU_CompileShader(GPU_VERTEX_SHADER, vertSrc);
        printShaderLog(vertexShader);

        // Create and compile the fragment shader
//        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragmentShader, 1, &fragSrc, NULL);
//        glCompileShader(fragmentShader);
        fragmentShader = GPU_CompileShader(GPU_FRAGMENT_SHADER, fragSrc);
        printShaderLog(fragmentShader);

        // Link the vertex and fragment shader into a shader program
//        shaderProgram = glCreateProgram();
//        glAttachShader(shaderProgram, vertexShader);
//        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
//        glLinkProgram(shaderProgram);
        shaderProgram = GPU_LinkShaders(vertexShader, fragmentShader);

        printProgramLog(shaderProgram);
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
        GPU_FlushBlitBuffer();

        GPU_MatrixMode(GPU_MODELVIEW);
        GPU_PushMatrix();
        GPU_LoadIdentity();
        GPU_MatrixMode(GPU_PROJECTION);
        GPU_PushMatrix();
        GPU_LoadIdentity();

        glViewport(0, 0, gameBounds.w, gameBounds.h);


//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glDisable(GL_DEPTH_TEST);

        // Bind our framebuffer and clear the screen to white
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // <<<<---------------------------------------------------------------------------------------------------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void end_3d()
    {
        glViewport(0, 0, screenViewport[2], screenViewport[3]);

        GPU_PushMatrix();
        GPU_LoadIdentity();
//        GPU_Rotate(20, 0.707, 0.707, 0);

        // Bind default framebuffer and clear the screen to cornflower blue
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // <<<<---------------------------------------------------------------------------------------------------

//        glClearColor(0.392f, 0.584f, 0.929f, 0.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vaoQuad);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(panelShaderProgram);

        float mvp[16];
        GPU_GetModelViewProjection(mvp);
        glUniformMatrix4fv(modelViewProjection_loc, 1, 0, mvp);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        //glUniform2f(uniTexOffset, 1.0f / 300.0f, 0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        GPU_PopMatrix();

        GPU_ResetRendererState();

        GPU_MatrixMode(GPU_MODELVIEW);
        GPU_PopMatrix();
        GPU_MatrixMode(GPU_PROJECTION);
        GPU_PopMatrix();
    }

    void draw_3d_stuff() {
        begin_3d();

        GLfloat gldata[21];
        float mvp[16];
        float t = SDL_GetTicks()/1000.0f;

        GPU_Rotate(50*t, 0, 0.707, 0.707);
        GPU_Rotate(100*t, 0.707, 0.707, 0);

        gldata[0] = 0;
        gldata[1] = 0.2f;
        gldata[2] = 0;

        gldata[3] = 1.0f;
        gldata[4] = 0.0f;
        gldata[5] = 0.0f;
        gldata[6] = 1.0f;

        gldata[7] = -0.2f;
        gldata[8] = -0.2f;
        gldata[9] = 0;

        gldata[10] = 0.0f;
        gldata[11] = 1.0f;
        gldata[12] = 0.0f;
        gldata[13] = 1.0f;

        gldata[14] = 0.2f;
        gldata[15] = -0.2f;
        gldata[16] = 0;

        gldata[17] = 0.0f;
        gldata[18] = 0.0f;
        gldata[19] = 1.0f;
        gldata[20] = 1.0f;

        glUseProgram(p);
        glBindVertexArray(VAO);

        GPU_GetModelViewProjection(mvp);
        glUniformMatrix4fv(modelViewProjection_loc, 1, 0, mvp);

        glEnableVertexAttribArray(vertex_loc);
        glEnableVertexAttribArray(color_loc);


        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gldata), gldata, GL_STREAM_DRAW);

        glVertexAttribPointer(
                              vertex_loc,
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalize
                              sizeof(float)*7,    // stride
                              (void*)0            // offset
                              );

        glVertexAttribPointer(
                              color_loc,
                              4,                      // size
                              GL_FLOAT,               // type
                              GL_FALSE,               // normalize
                              sizeof(float)*7,        // stride
                              (void*)(sizeof(float)*3)  // offset
                              );

        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(color_loc);
        glDisableVertexAttribArray(vertex_loc);
        
        end_3d();
    }
};

#endif /* OpenGLRenderer_h */
