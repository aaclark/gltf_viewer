// Model viewer code for the assignments in Computer Graphics 1TD388/1MD150.
//
// Modify this and other source files according to the tasks in the instructions.
//

#include "gltf_io.h"
#include "gltf_scene.h"
#include "gltf_render.h"
#include "cg_utils.h"
#include "cg_trackball.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>

// Struct for our application context
struct Context {
    int width = 512;
    int height = 512;
    GLFWwindow *window;
    gltf::GLTFAsset asset;
    gltf::DrawableList drawables;
    GLuint program;
    GLuint emptyVAO;
    float elapsedTime;
    std::string gltfFilename = "armadillo.gltf"; //Assignment2
    // Display normals
    bool display_normals = false;
    // Lighting
    glm::vec3 background_color = glm::vec3(0.5f);
    glm::vec3 ambient_color = glm::vec3(0.0f,0.0f,0.16f);
    glm::vec3 diffuse_color = glm::vec3(0.6f,0.0f,0.0f);
    glm::vec3 specular_color = glm::vec3(1.0f,1.0f,0.0f);
    float specular_power = 20.0f;
    glm::vec3 light_position = glm::vec3(1.0f);
    // Model-space transform
    glm::vec3 log_scale = glm::vec3(0.0f);
    glm::vec3 rotate_angle = glm::vec3(0.0f);
    glm::vec3 translate = glm::vec3(0.0f);
    // Enable/Disable MVP
    bool enable_view_transform = true;
    bool enable_projection = true;
    // Input
    bool enable_trackball = true;
    cg::Trackball trackball;
    // Scroll zoom
    float scroll_zoom_fov = 0.0f;

    // Edit-in-place flags
    bool edit_model_transform = false;
    bool edit_model_scale = false;
    bool edit_model_rotate = false;
    bool edit_model_translate = false;
    bool edit_lights = false;
};

// Returns the absolute path to the src/shader directory
std::string shader_dir(void)
{
    std::string rootDir = cg::get_env_var("MODEL_VIEWER_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: MODEL_VIEWER_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/src/shaders/";
}

// Returns the absolute path to the assets/gltf directory
std::string gltf_dir(void)
{
    std::string rootDir = cg::get_env_var("MODEL_VIEWER_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: MODEL_VIEWER_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/assets/gltf/";
}

void do_initialization(Context &ctx)
{
    ctx.program = cg::load_shader_program(
        shader_dir() + "mesh.vert",
        shader_dir() + "mesh.frag");

    gltf::load_gltf_asset(ctx.gltfFilename, gltf_dir(), ctx.asset);
    gltf::create_drawables_from_gltf_asset(ctx.drawables, ctx.asset);
}

void draw_scene(Context &ctx)
{
    // Activate shader program
    glUseProgram(ctx.program);

    // Set render state
    glEnable(GL_DEPTH_TEST);  // Enable Z-buffering

    // Define per-scene uniforms
    glUniform1f(glGetUniformLocation(ctx.program, "u_time"), ctx.elapsedTime);

    // Declare transformation matrices (Model-View-Project)
    auto modelToWorld = glm::mat4(1.0f);
    auto worldToView = glm::mat4(1.0f);
    auto viewToProjection = glm::mat4(1.0f);

    /*
     * The normalized device space is a unique cube, with the left, bottom, near of (-1, -1, -1)
     * and the right, top, far of (1, 1, 1). Hence, the first component of the vertex coordinate (x)
     * defines the position from the left (-1) to the right (1). The 2nd component (y) defines the
     * position form the bottom (-1) to the top (1) and the 3rd component (z) defines the depth form
     * near (-1) to far (1). Thus, the "up-vector" is (0, 1, 0).
    */

    // Model transformations: Translation -> Rotation -> Scale
    if (ctx.edit_model_transform) {
        if (ctx.edit_model_translate) {
            modelToWorld = glm::translate(modelToWorld, ctx.translate);
        }

        if (ctx.edit_model_rotate) {
            modelToWorld = glm::rotate(modelToWorld,
                glm::radians(ctx.rotate_angle[0]),
                glm::vec3(-1.0f, 0.0f, 0.0f));
            modelToWorld = glm::rotate(modelToWorld,
                glm::radians(ctx.rotate_angle[1]),
                glm::vec3(0.0f, 1.0f, 0.0f));
            modelToWorld = glm::rotate(modelToWorld,
                glm::radians(ctx.rotate_angle[2]),
                glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (ctx.edit_model_scale) {
            modelToWorld = glm::scale(modelToWorld, glm::exp(ctx.log_scale));
        }
    }

    // Change of frame
    if (ctx.enable_view_transform) {
        // Mouse input (trackball)
        if (ctx.enable_trackball) {
            worldToView *= glm::mat4(ctx.trackball.orient);
        }
        worldToView *= glm::lookAt( // somehow includes clipping
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Camera
    glfwGetWindowSize(ctx.window, &ctx.width, &ctx.height);
    const float aspect = ((float)ctx.width)/(float)ctx.height;
    if (ctx.enable_projection) {
        viewToProjection *= glm::perspective(glm::radians(30.0f * (1.0f + ctx.scroll_zoom_fov)), aspect, 0.1f, 10.0f);
    } else {
        viewToProjection *= glm::ortho(-aspect,aspect,-1.0f,1.0f, 0.1f, 10.0f);
    }

    // Draw scene
    for (unsigned i = 0; i < ctx.asset.nodes.size(); ++i) {
        const gltf::Node &node = ctx.asset.nodes[i];
        const gltf::Drawable &drawable = ctx.drawables[node.mesh];


        // Model-space transformations
        glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_model"), 1,
            GL_FALSE, &modelToWorld[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_view"), 1,
            GL_FALSE, &worldToView[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_projection"), 1,
            GL_FALSE, &viewToProjection[0][0]);


        // Display Normals (hack)
        glUniform1f(glGetUniformLocation(ctx.program, "u_display_normal"), (float) ctx.display_normals);


        // Lighting
        if (!ctx.display_normals) {
            glUniform3fv(glGetUniformLocation(ctx.program, "u_ambientColor"), 1,
                &ctx.ambient_color[0]);
            glUniform3fv(glGetUniformLocation(ctx.program, "u_diffuseColor"), 1,
                &ctx.diffuse_color[0]);
            glUniform3fv(glGetUniformLocation(ctx.program, "u_specularColor"), 1,
                &ctx.specular_color[0]);
            glUniform1fv(glGetUniformLocation(ctx.program, "u_specularPower"), 1,
                &ctx.specular_power);
            glUniform3fv(glGetUniformLocation(ctx.program, "u_lightPosition"), 1,
                &ctx.light_position[0]);
        }

        // Draw object
        glBindVertexArray(drawable.vao);
        glDrawElements(GL_TRIANGLES, drawable.indexCount, drawable.indexType,
                       (GLvoid *)(intptr_t)drawable.indexByteOffset);
        glBindVertexArray(0);
    }

    // Clean up
    cg::reset_gl_render_state();
    glUseProgram(0);
}

void do_rendering(Context &ctx)
{
    // Clear render states at the start of each frame
    cg::reset_gl_render_state();

    // Clear color and depth buffers
    glClearColor(ctx.background_color[0], ctx.background_color[1], ctx.background_color[2], 1.0f); //
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_scene(ctx);
}

void reload_shaders(Context *ctx)
{
    glDeleteProgram(ctx->program);
    ctx->program = cg::load_shader_program(shader_dir() + "mesh.vert", shader_dir() + "mesh.frag");
}

void error_callback(int /*error*/, const char *description)
{
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_R && action == GLFW_PRESS) { reload_shaders(ctx); }
}

void char_callback(GLFWwindow *window, unsigned int codepoint)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    if (ImGui::GetIO().WantTextInput) return;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        ctx->trackball.center = glm::vec2(x, y);
        ctx->trackball.tracking = (action == GLFW_PRESS);
    }
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
    // Forward event to ImGui
    if (ImGui::GetIO().WantCaptureMouse) return;

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    cg::trackball_move(ctx->trackball, float(x), float(y));
}

void scroll_callback(GLFWwindow *window, double x, double y)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if (ImGui::GetIO().WantCaptureMouse) return;

    float scroll_scale = 0.1f;

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->scroll_zoom_fov = glm::clamp(ctx->scroll_zoom_fov + (float)(y * scroll_scale), 0.0f, 3.0f);
}

void resize_callback(GLFWwindow *window, int width, int height)
{
    // Update window size and viewport rectangle
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->width = width;
    ctx->height = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    Context ctx = Context();
    if (argc > 1) { ctx.gltfFilename = std::string(argv[1]); }

    // Create a GLFW window
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ctx.window = glfwCreateWindow(ctx.width, ctx.height, "Model viewer", nullptr, nullptr);
    glfwMakeContextCurrent(ctx.window);
    glfwSetWindowUserPointer(ctx.window, &ctx);
    glfwSetKeyCallback(ctx.window, key_callback);
    glfwSetCharCallback(ctx.window, char_callback);
    glfwSetMouseButtonCallback(ctx.window, mouse_button_callback);
    glfwSetCursorPosCallback(ctx.window, cursor_pos_callback);
    glfwSetScrollCallback(ctx.window, scroll_callback);
    glfwSetFramebufferSizeCallback(ctx.window, resize_callback);

    // Load OpenGL functions
    if (gl3wInit() || !gl3wIsSupported(3, 3) /*check OpenGL version*/) {
        std::cerr << "Error: failed to initialize OpenGL" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(ctx.window, false /*do not install callbacks*/);
    ImGui_ImplOpenGL3_Init("#version 330" /*GLSL version*/);

    // Initialize rendering
    glGenVertexArrays(1, &ctx.emptyVAO);
    glBindVertexArray(ctx.emptyVAO);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    do_initialization(ctx);

    // Start rendering loop
    while (!glfwWindowShouldClose(ctx.window)) {
        glfwPollEvents();
        ctx.elapsedTime = glfwGetTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // TEST Window
        // ImGui::ShowDemoWindow();

        ImGui::Begin("Settings");
        {

            ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

            ImGui::ColorEdit3("Background Color", &ctx.background_color[0]);

            ImGui::Checkbox("Display Normals", &ctx.display_normals);
            if (ctx.display_normals) {
                // Anything useful to do here?
            } else {
                ImGui::Checkbox("Edit Lights", &ctx.edit_lights);
                if (ctx.edit_lights) {
                    ImGui::ColorEdit3("Ambient Color", &ctx.ambient_color[0]);
                    ImGui::ColorEdit3("Diffuse Color", &ctx.diffuse_color[0]);
                    ImGui::ColorEdit3("Specular Color", &ctx.specular_color[0]);
                    ImGui::SliderFloat("Specular Power", &ctx.specular_power, 1.0f, 40.0f);
                    ImGui::SliderFloat3("Light Position", &ctx.light_position[0], -4.f, 4.f);
                }
            }

            ImGui::Checkbox("Edit Model Transform", &ctx.edit_model_transform);
            if (ctx.edit_model_transform) {

                ImGui::Checkbox("Enable Scale", &ctx.edit_model_scale);
                if (ctx.edit_model_scale)
                    ImGui::SliderFloat3("(log) Scale X/Y/Z", &ctx.log_scale[0], -1.0f, 1.0f);

                ImGui::Checkbox("Enable Rotate", &ctx.edit_model_rotate);
                if (ctx.edit_model_rotate)
                    ImGui::SliderFloat3("Rotate Deg. About X/Y/Z", &ctx.rotate_angle[0], -180.0f, 180.0f);

                ImGui::Checkbox("Enable Translate", &ctx.edit_model_translate);
                if (ctx.edit_model_translate)
                    ImGui::SliderFloat3("Translate X/Y/Z", &ctx.translate[0], -1.0f, 1.0f);

                // ImGui::SliderFloat("Origin Z-Offset", &ctx.z_offset, 0.0f, 10.0f);

            }

            ImGui::Checkbox("Use Projection Camera", &ctx.enable_projection);
            // ImGui::Checkbox("Enable View Transform", &ctx.enable_view_transform);
            // if (ctx.enable_view_transform) {
            //     // ImGui::Checkbox("Enable Trackball", &ctx.enable_trackball);
            //     // ImGui::Checkbox("Use Projection Camera", &ctx.enable_projection);
            // }

        }
        ImGui::End();

        do_rendering(ctx);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(ctx.window);
    }

    // Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
    std::exit(EXIT_SUCCESS);
}
