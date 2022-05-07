#pragma once

#include <glm/fwd.hpp>
#include <fstream>
#include <iostream>

#include "stb_image.cpp"

#include "Cherno_OpenGL_Library/Renderer.cpp"
#include "Cherno_OpenGL_Library/Renderer.h"
#include "Cherno_OpenGL_Library/Shader.cpp"
#include "Cherno_OpenGL_Library/VertexBuffer.cpp"
#include "Cherno_OpenGL_Library/IndexBuffer.cpp"
#include "Cherno_OpenGL_Library/VertexArray.cpp"
#include "Cherno_OpenGL_Library/VertexBufferLayout.cpp"
#include "Cherno_OpenGL_Library/Texture.cpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

int number_of_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1; //hardware_concurrency can fail and give 0
constexpr float epsilon = pow(10, -5);
