#version 150

in vec3 gpu_Vertex;
in vec3 gpu_Normal;
in vec3 gpu_Color;
uniform mat4 gpu_ModelMatrix;
uniform mat4 gpu_ModelViewProjectionMatrix;

out vec4 color;

struct Light {
    vec3 color;
    vec3 angle;
    float intensity;
};

void main(void)
{
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex, 1.0);
    vec3 normal = normalize(gpu_ModelMatrix * vec4(gpu_Normal, 1.0)).xyz;

//    // Position of the vertex, in worldspace : M * position
//    Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
//
//    // Vector that goes from the vertex to the camera, in camera space.
//    // In camera space, the camera is at the origin (0,0,0).
//    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
//    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
//
//    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
//    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
//    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
//
//    // Normal of the the vertex, in camera space
//    Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

//    float intensity = dot(gpu_ModelMatrix * vec4(gpu_Normal, 1.0), vec4(-1.0, -1.0, 1.0, 1.0));

    Light lights[2];
    lights[0] = Light(
                      vec3(1.0,0.95,0.8),
                      normalize(vec3(-1,1,1)),
                      1.0
                      );
    lights[1] = Light(
                      vec3(1.0,0.85,0.7),
                      normalize(vec3(1,-1,1)),
                      0.66
                      );

    color = vec4(0.0);
    vec4 ambient = vec4(vec3(0.05), 0.0);

    for (int i = 0; i < 2; i++) {
        float incidence = clamp(dot(normal, lights[i].angle), 0, 1);
        color += ambient + lights[i].intensity * vec4(gpu_Color, 1.0) * vec4(incidence * lights[i].color, 1.0);
    }
}
//#version 120
//
//attribute vec3 gpu_Vertex;
//attribute vec4 gpu_Color;
//uniform mat4 gpu_ModelViewProjectionMatrix;
//
//varying vec4 color;
//
//void main(void)
//{
//    color = gpu_Color;
//    gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex, 1.0);
//}
