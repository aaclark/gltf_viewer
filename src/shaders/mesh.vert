#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
//uniform float u_time;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
// Lighting: Color
//uniform vec3 u_ambient_color;
//uniform vec3 u_diffuse_color;
//uniform vec3 u_specular_color;
// Lighting: Params
//uniform float u_specular_power;
uniform vec3 u_light_position;
// Fade-in Normal-colors
//uniform float u_display_normal = 0.0;
// Display Gamma
//uniform float u_gamma;

// Vertex inputs (attributes from vertex buffers)
// Note: we often declare the position attribute as vec4 in the
// vertex shader, even if the actual type in the buffer is
// vec3. This is valid and will give us a homogenous coordinate
// with the last component assigned the value 1.
layout(location = 0) in vec4 a_position; // input vertex position
layout(location = 1) in vec4 a_color; // input vertex color (RGB/A)
layout(location = 2) in vec3 a_normal; // input vertex normal
layout(location = 3) in vec2 a_texture; //
// Buffer size 12, here it's 13
// IMPORTANT: layout(location = x) specifies buffer indices

// Vertex shader outputs
out vec4 v_color; // output vertex color
out vec3 N;
out vec3 L;
out vec3 H;
out vec3 V;

void main()
{

    mat4 mv = u_view * u_model;

    // Transform the vertex position to view space (eye coordinates)
    vec3 position_eye = vec3(mv * a_position);

    // Calculate the view-space normal
    N = normalize(mat3(mv) * a_normal);

    // Calculate the view-space light direction
    L = normalize(u_light_position - position_eye);

    // Half-vector
    H = normalize(u_light_position + position_eye);

    // View-vector (a_position in view space)
    V = -position_eye;

    gl_Position = (u_projection * mv) * a_position;
}
