#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform sampler2D foo;
uniform float glow_radius = 1.0;
uniform float glow_intensity = 1.0;
uniform vec3 glow_rgb = vec3(0,0,0);

// Fragment shader inputs
in vec4 v_color;

// Fragment shader outputs
out vec4 frag_color;

void main()
{
    frag_color = v_color;
}
