#pragma once
#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <math.h>
#include <glm/fwd.hpp>
#include "Cherno_OpenGL_Library/Renderer.h"
#include "Cherno_OpenGL_Library/Shader.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include "Cherno_OpenGL_Library/IndexBuffer.h"
#include "Cherno_OpenGL_Library/VertexArray.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
// #include "Cherno_OpenGL_Library/Texture.cpp"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
class Scene{
private:
    //attributes
    float min_x, max_x, min_z,max_z;
    float m_matrix_theta_rad;
    int N;
    float marching_cubes_cube_length_x, marching_cubes_cube_length_y, marching_cubes_cube_length_z;
    float shrink_input_factor, scale_output_factor, increase_output_value;
    float (*smoothStep)(float, float, float), (*a_i_j)(float, float);
    
    std::set<int> deactivated_n;
    std::vector<std::thread> workers;
    std::vector< float* > vertex_buffers_per_thread;
    std::vector< int > vertices_per_thread;
    std::vector< unsigned int * > index_buffers_per_thread;
    std::vector< int > indexs_per_thread;
    std::vector< bool > thread_done_status;
    std::vector< bool > terminate_now; 

    std::mutex vertx_indx_buffr_mtx;
    VertexBufferLayout m_layout;

    //functions
    void worker_thread_function(int my_thread_id);
    void worker_thread_update_state(int its_thread_id,const std::vector<float> &temp_vertex_buffer, const std::vector<unsigned int> &temp_index_buffer);

    //static attributes 

    static const int edges_marching_cubes[256];
    static const int triangulation[256][16];
    static const int cornerIndexAFromEdge[12];
    static const int cornerIndexBFromEdge[12];
    static const int number_of_threads;
    static const std::string vertex_shader;
    static const std::string fragment_shader;

public:
    Scene(float min_x,float max_x,float min_z,float max_z, float (*smoothStep)(float, float, float), float (*a_i_j)(float, float), float m_matrix_theta, int N, float marching_cubes_cube_length_x,float marching_cubes_cube_length_y ,float marching_cubes_cube_length_z, float shrink_input_factor, float scale_output_factor,float increase_output_value);
    ~Scene();
    void block_untill_all_worker_threads_finish(int millisoncds_per_polling);
    void render(glm::mat4 &mvp);
    void print_state();
    void dump_obj(int thread_id, const char filename[]);
    void dump_obj(const char *filename);

}; 