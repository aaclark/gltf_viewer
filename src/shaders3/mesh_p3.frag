#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform float dpulse = 2.0; // Cycle rate

// Fragment shader inputs
in vec3 v_color;

// Fragment shader outputs
out vec3 frag_color;

void main()
{
    frag_color = vec3(1.0, 1.0*sin(u_time*dpulse), 0.5*cos(u_time*dpulse)); // Pleasant glowing triangle
}
