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
uniform float u_ambient_factor;
uniform float u_specular_power;
// Display Gamma
uniform float u_gamma;
// Texture
uniform samplerCube u_cubemap;
uniform float u_reflectance_value;
// Other junk...
//uniform sampler2D foo;

// Fragment shader inputs
in vec4 v_color;
in vec3 N; // Normal
in vec3 L; // Light
in vec3 H; // Half-angle
in vec3 V; // View

// Fragment shader outputs
out vec3 frag_color;

void main()
{
    // Reflection vector (-view | normal)
    vec3 R = reflect(-V, N);

    // Texture lookup
//    vec3 cubemap_color = texture(u_cubemap, R).rgb;

    // There is no scalar factor to ambient
    float ambient = 1.0;

    // Calculate the diffuse (Lambertian) reflection term
    float diffuse = max(0.0, dot(N, L));

    // Specular term of the Blinn-Phong shading model
    float specular = ((u_specular_power + 8.0f)/8.0f)*pow(max(0.0, dot(N, H)), u_specular_power);

    // Render
    vec3 L_a = (ambient * u_ambient_color);
    vec3 L_d = (diffuse * u_diffuse_color);
    vec3 L_s = (specular * u_specular_color);
    vec3 surface = L_a + L_d + L_s;
    float a = (1.0-u_reflectance_value);
    float b = (u_reflectance_value);
    vec3 reflection = (texture(u_cubemap, R).rgb);

    // Interpolate
    frag_color.rgb = (a+1)*surface + b*reflection;

    // apply gamma correction
    frag_color.rgb = pow(frag_color.rgb, vec3(1.0/u_gamma));

}
