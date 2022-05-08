#pragma once
#include <bits/stdc++.h>
#include <thread>
#include <math.h>
class Scene{
private:
    //attributes
    float min_x, max_x, min_z,max_z;
    float m_matrix_theta_rad;
    int N;
    float marching_cubes_cube_length;
    
    float (*smoothStep)(float, float, float), (*a_i_j)(float, float);
    
    std::set<int> deactivated_n;
    std::vector<std::thread> workers;
    std::vector< float* > vertex_buffers_per_thread;
    std::vector< int > vertices_per_thread;
    std::vector< unsigned int * > index_buffers_per_thread;
    std::vector< int > indexs_per_thread;
    std::vector< bool > thread_done_status;
    std::vector< bool > terminate_now; 

    // pthread_mutex_t curve_description;

    //functions
    void worker_thread_function(int my_thread_id);
    void worker_thread_update_state(int its_thread_id,const std::vector<float> &temp_vertex_buffer, const std::vector<unsigned int> &temp_index_buffer);

    //static attributes 

    static const int edges_marching_cubes[256];
    static const int triangulation[256][16];
    static const int cornerIndexAFromEdge[12];
    static const int cornerIndexBFromEdge[12];
    static const int number_of_threads;

public:
    Scene(float min_x,float max_x,float min_z,float max_z, float (*smoothStep)(float, float, float), float (*a_i_j)(float, float), float m_matrix_theta, int N, float marching_cubes_cube_length);
    ~Scene();
    void block_untill_all_worker_threads_finish(int millisoncds_per_polling);
}; 
