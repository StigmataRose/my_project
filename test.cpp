#include <visage/app.h>
#include <visage/graphics.h>
#include <visage/utils.h>
#include <visage/widgets.h>
#include "visage/windowing.h"
#include <memory>       // For std::unique_ptr
#include <string>       // For std::string
#include <fstream>      // For std::ifstream
#include <stdexcept>    // For std::runtime_error
#include <streambuf>    // For std::istreambuf_iterator

// Helper function to read a text file into a string
std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Error: Could not open file: " + filepath);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

/**
 * @class ShaderBackgroundApp
 * @brief A Visage application window that uses a GLSL shader for its background.
 */
class ShaderBackgroundApp : public visage::ApplicationWindow {
public:
    ShaderBackgroundApp() {
        setTitle("Visage Shader Background");

        onDraw() = [this](visage::Canvas& canvas) {
            // Intentionally blank. The post effect draws the background.
        };

        setupShaderEffect();
    }

    /**
     * @brief Initializes the ShaderPostEffect and applies it to this window.
     */
    void setupShaderEffect() {
        // Load the shader source code from external files
        std::string vertex_shader_source = read_file("passthrough.vert.glsl");
        std::string fragment_shader_source = read_file("shader.frag.glsl");

        // Use the loaded strings to create the shader effect.
        // We use .c_str() and .length() to get the required data from the std::string.
        shader_effect_ = std::make_unique<visage::ShaderPostEffect>(
            visage::EmbeddedFile{
                "passthrough.vert.glsl",                                                  // 1. name (const char*)
                reinterpret_cast<const char*>(vertex_shader_source.c_str()),     // 2. data (const unsigned char*)
                static_cast<int>(vertex_shader_source.length())                           // 3. size (int)
            },
            visage::EmbeddedFile{
                "shader.frag.glsl",                                                       // 1. name (const char*)
                reinterpret_cast<const char*>(fragment_shader_source.c_str()),   // 2. data (const unsigned char*)
                static_cast<int>(fragment_shader_source.length())                         // 3. size (int)
            }
        );

        // Set the shader as the post-effect for the entire window.
        setPostEffect(shader_effect_.get());
    }

    /**
     * @brief Shows the window and starts the main application event loop.
     */
    void launch() {
        show(800, 600); // Show window with an initial size of 800x600
        runEventLoop();
    }

private:
    std::unique_ptr<visage::ShaderPostEffect> shader_effect_;
};

// The main entry point of the program
int main() {

    visage::ShaderCompiler compiler;
    try {
        ShaderBackgroundApp app;
        app.launch();
    } catch (const std::exception& e) {
        // Catch potential file loading errors and print them
        fprintf(stderr, "An exception occurred: %s\n", e.what());
        return 1;
    }
    return 0;
}