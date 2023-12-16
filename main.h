#pragma once

#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <iostream>


#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>


#include "OpenGL/Renderer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Camera.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Texture.h"
#include "OpenGL/Skybox.hpp"
// #include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "FOSS_Code/json.hpp"
using json = nlohmann::json;

#include "Source/plane.h"
// #include "Source/terrain.h"
// #include "Source/trees.h"
// #include "Source/cloud.h"
#include "Source/Grid.h"
