#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
// Lighting: Color
uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
// Lighting: Params
uniform float u_specularPower;
uniform vec3 u_lightPosition;
// Fade-in Normal-colors
uniform float u_display_normal;

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

void main()
{

    mat4 mv = u_view * u_model;

    // Transform the vertex position to view space (eye coordinates)
    vec3 positionEye = vec3(mv * a_position);

    // Calculate the view-space normal
    vec3 N = normalize(mat3(mv) * a_normal);

    // Calculate the view-space light direction
    vec3 L = normalize(u_lightPosition - positionEye);

    // Half-vector
    vec3 H = normalize(u_lightPosition + positionEye);

    // There is no scalar factor to ambient
    float ambient = 1.0;

    // Calculate the diffuse (Lambertian) reflection term
    float diffuse = max(0.0, dot(N, L));

    // Specular term of the Blinn-Phong shading model
    float specular_angle = max(0.0, dot(N, H));
    float specular = pow(specular_angle, u_specularPower);

    // Multiply the diffuse reflection term with the base surface color
    v_color = (
        (ambient * vec4(u_ambientColor, 1.0))
        + (diffuse * vec4(u_diffuseColor, 1.0))
        + (specular * vec4(u_specularColor, 1.0))
    )*(1.0 - u_display_normal)
    + vec4(a_normal, 1.0f)*(u_display_normal);

    // Debugging POS
//     v_color = a_position;

    gl_Position = (u_projection * mv) * a_position;
}
