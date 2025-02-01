#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
// ...

// Fragment shader inputs
in vec3 v_color;

// Fragment shader outputs
out vec3 frag_color;

void main()
{
    frag_color = v_color; // From vert
}
