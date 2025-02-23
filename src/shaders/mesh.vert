#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

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

mat4 v_mvp;

void main()
{
    v_color = a_position;//vec4(a_normal * 0.5 + 0.5, 1.0); // transforms from [-1,1] to [0,1]
    gl_Position = (u_projection * u_view * u_model) * vec4(a_position.xyz, 1.0);
}
