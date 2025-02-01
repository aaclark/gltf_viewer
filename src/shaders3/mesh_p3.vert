#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform float dtheta = 0.3; // Rotation speed
uniform float off_x = 0.0; // Offset from origin
uniform float off_y = 0.1; // Offset from origin

// Vertex inputs (attributes from vertex buffers)
layout(location = 0) in vec4 a_position;
in vec3 a_color; // input vertex color (RGB)

// Vertex shader outputs
out vec3 v_color; // output vertex color

void main()
{
    v_color = a_color;
    gl_Position = vec4(
    //a_position.x,
    (cos(u_time*dtheta)*(a_position.x+off_x)) + (-sin(u_time*dtheta)*(a_position.y+off_y)),
    //a_position.y,
    (sin(u_time*dtheta)*(a_position.x+off_x)) + (cos(u_time*dtheta)*(a_position.y+off_y)),
    a_position.z,
    1.0);
}
