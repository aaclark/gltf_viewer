#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
//uniform mat4 u_model;
//uniform mat4 u_view;
//uniform mat4 u_projection;
// Lighting: Color
uniform vec3 u_ambient_color;
uniform vec3 u_diffuse_color;
uniform vec3 u_specular_color;
// Lighting: Params
uniform float u_specular_power;
//uniform vec3 u_light_position;
// Fade-in Normal-colors
uniform float u_display_normal = 0.0;
// Display Gamma
uniform float u_gamma;

// Uniform constants
uniform sampler2D foo;
uniform float glow_radius = 1.0;
uniform float glow_intensity = 1.0;
uniform vec3 glow_rgb = vec3(0,0,0);

// Fragment shader inputs
in vec4 v_color;
in vec3 N;
in vec3 L;
in vec3 H;
in vec3 V;

// Fragment shader outputs
out vec4 frag_color;

void main()
{

    // There is no scalar factor to ambient
    float ambient = 1.0;

    // Calculate the diffuse (Lambertian) reflection term
    float diffuse = max(0.0, dot(N, L));

    // Specular term of the Blinn-Phong shading model
    float specular = ((u_specular_power + 8.0f)/8.0f)*pow(max(0.0, dot(N, H)), u_specular_power);

    // Multiply the diffuse reflection term with the base surface color
    frag_color =(
    (ambient * vec4(u_ambient_color, 1.0))
    +(diffuse * vec4(u_diffuse_color, 1.0))
    +(specular * vec4(u_specular_color, 1.0)));
    //    v_color = (u_display_normal)*(vec4(N, 1.0));

    // apply gamma correction
    frag_color.rgb = pow(frag_color.rgb, vec3(1.0/u_gamma));

}
