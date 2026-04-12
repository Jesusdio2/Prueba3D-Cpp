#include "splash_renderer.h"
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SplashRenderer", __VA_ARGS__)

const char* vertexShaderSource = R"glsl(#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(#version 300 es
precision mediump float;

in vec2 TexCoord;
uniform sampler2D ourTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(ourTexture, TexCoord);
}
)glsl";

SplashRenderer::SplashRenderer() {}
SplashRenderer::~SplashRenderer() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

unsigned int SplashRenderer::CompileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOGE("ERROR::SHADER::COMPILATION_FAILED\n%s", infoLog);
    }
    return shader;
}

unsigned int SplashRenderer::CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOGE("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void SplashRenderer::Init() {
    shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Quad coordinates (x, y, u, v) - full screen quad
    float vertices[] = {
        // Positions   // Texture Coords
        -1.0f,  1.0f,  0.0f, 0.0f, // Top-left
        -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left
         1.0f,  1.0f,  1.0f, 0.0f, // Top-right
         1.0f, -1.0f,  1.0f, 1.0f  // Bottom-right
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SplashRenderer::Render(SplashTexture& tex) {
    glClearColor(127/255.0f, 127/255.0f, 127/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (shaderProgram == 0 || tex.id == 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
