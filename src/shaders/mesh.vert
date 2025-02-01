#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_view;

// Vertex inputs (attributes from vertex buffers)
in vec4 a_position; // input vertex position ; Default: layout(location = 0) in vec4 a_position
in vec4 a_normal; // input vertex normal
in vec3 a_color; // input vertex color (RGB)

// Vertex shader outputs
out vec3 v_color; // output vertex color

void main()
{
    v_color = a_color;
    gl_Position = u_view*vec4(a_position.xyz, 1.0);
}
