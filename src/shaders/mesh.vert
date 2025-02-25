#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
// Lighting
uniform vec3 u_diffuseColor; // The diffuse surface color of the model
uniform vec3 u_lightPosition; // The position of your light source

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

mat4 mv;

void main()
{

    mv = u_view * u_model;

    // Transform the vertex position to view space (eye coordinates)
//    vec3 positionEye = vec3(mv * a_position);

    // Calculate the view-space normal
//    vec3 N = normalize(mat3(mv) * a_normal);

    // Calculate the view-space light direction
//    vec3 L = normalize(u_lightPosition - positionEye);

    // Calculate the diffuse (Lambertian) reflection term
//    float diffuse = max(0.0, dot(N, L));

    // Multiply the diffuse reflection term with the base surface color
//    v_color = diffuse * vec4(u_diffuseColor, 1.0);

    // Debugging POS
     v_color = a_position;

    gl_Position = (u_projection * mv) * a_position;
}
