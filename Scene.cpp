#include "Scene.h"

Scene::Scene ( float min_x,float max_x,float min_z,float max_z, float (*smoothStep)(float, float, float), float (*a_i_j)(float, float), float m_matrix_theta_rad, int N, float marching_cubes_cube_length){
    this->min_x = min_x;
    this->max_x = max_x;
    this->min_z = min_z;
    this->max_z = max_z;
    this->smoothStep = smoothStep;
    this->a_i_j = a_i_j;
    this->m_matrix_theta_rad = m_matrix_theta_rad;
    this->N = N;
    this->marching_cubes_cube_length = marching_cubes_cube_length;

    vertex_buffers_per_thread.resize(number_of_threads, nullptr);
    vertices_per_thread.resize(number_of_threads, 0);
    index_buffers_per_thread.resize(number_of_threads, nullptr);
    indexs_per_thread.resize(number_of_threads, 0);
    thread_done_status.resize(number_of_threads, false);
    terminate_now.resize(number_of_threads, false);

    for(int i=0 ; i< number_of_threads ; i++)
        workers.emplace_back(&Scene::worker_thread_function, this, i);
    
}
Scene::~Scene(){
    std::fill(this->terminate_now.begin(), this->terminate_now.end(), true);
    for(int i=0 ; i<number_of_threads ; i++)
        this->workers.at(i).join();
    
}
void Scene::block_untill_all_worker_threads_finish(int millisoncds_per_polling){
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(millisoncds_per_polling));
        bool done = std::accumulate(this->thread_done_status.begin(), this->thread_done_status.end(), true, [](bool left, bool right){ return left && right;});
        if(done)
            return;
    }
}

void Scene::worker_thread_function(const int my_thread_id){
    std::cout<<"Worker Thread " << my_thread_id << "has launched." <<std::endl;
    constexpr float epsilon = 0.0000000001;
    auto Noise_Function = [ this ](const float x, const float z){
        float ans_ = 0;

        for(int rotation_and_amp_factor = 0 ; rotation_and_amp_factor < this->N ; rotation_and_amp_factor++){
            if(this->deactivated_n.find(rotation_and_amp_factor) == this->deactivated_n.end()){
                float x_this_iter = ( 1 << rotation_and_amp_factor ) * cos(this->m_matrix_theta_rad * rotation_and_amp_factor) * x - sin(this->m_matrix_theta_rad * rotation_and_amp_factor ) * z;
                float z_this_iter = ( 1 << rotation_and_amp_factor ) * sin(this->m_matrix_theta_rad * rotation_and_amp_factor) * x + cos(this->m_matrix_theta_rad * rotation_and_amp_factor ) * z;
                float x_frac_prt, z_frac_prt, i, j, a, b, c, d;
                x_frac_prt = modf(x_this_iter, &i);
                z_frac_prt = modf(z_this_iter, &j);
                a = a_i_j(i    , j    );
                b = a_i_j(i + 1, j    );
                c = a_i_j(i    , j + 1);
                d = a_i_j(i + 1, j + 1);
                float S_x = this->smoothStep(0, 1, x_frac_prt);
                float S_z = this->smoothStep(0, 1, z_frac_prt);
                
                ans_ += (a + ( b - a ) * S_x + ( c - a ) * S_z + ( a - b - c + d ) * S_x * S_z) / (1 << rotation_and_amp_factor);
            }
        }
        return ans_;
    };
    int edge_to_vertex_index_map[12];
    int vertex_index_to_edge_map[12];
    while(not terminate_now[my_thread_id]){
    
        if( not thread_done_status[my_thread_id] ){

            std::cout<<"Worker thread "<< my_thread_id << " has started working"<<std::endl;
            float delta_x = (this->max_x - this->min_x) / number_of_threads;
            float x_start = my_thread_id * delta_x, 
                x_end = (my_thread_id + 1) * delta_x;

            std::vector<float> temp_vertex_buffer;
            std::vector<unsigned int> temp_index_buffer;
    
            for(int iter_i = 0 ; x_start + iter_i * marching_cubes_cube_length < x_end ; iter_i++){
                float x_this_iter = x_start + iter_i * marching_cubes_cube_length;
                for( int iter_j = 0 ; this->min_z + iter_j * marching_cubes_cube_length < this->max_z ; iter_j++){
                    float z_this_iter = this->min_z + iter_j * marching_cubes_cube_length;
                    float y_vetrex_0 = Noise_Function(x_this_iter                             , z_this_iter                             );
                    float y_vetrex_1 = Noise_Function(x_this_iter + marching_cubes_cube_length, z_this_iter                             );
                    float y_vetrex_2 = Noise_Function(x_this_iter + marching_cubes_cube_length, z_this_iter + marching_cubes_cube_length);
                    float y_vetrex_3 = Noise_Function(x_this_iter                             , z_this_iter + marching_cubes_cube_length);

                    float max_y = std::max(std::max(y_vetrex_0,y_vetrex_1),std::max(y_vetrex_2,y_vetrex_3));
                    float min_y = std::min(std::min(y_vetrex_0,y_vetrex_1), std::min(y_vetrex_2,y_vetrex_3));

                    int number_of_vertical_cubes = ceil( ( (max_y - min_y) / marching_cubes_cube_length ) + epsilon); //need 1 cube even if max_y == min_y
                    
                    for( int iter_k = 0 ; iter_k < number_of_vertical_cubes ; iter_k++){
                        int marching_cubes_key = 0;
                        if( min_y + iter_k * marching_cubes_cube_length <= y_vetrex_0 )         marching_cubes_key |= (1 << 0);
                        if( min_y + iter_k * marching_cubes_cube_length <= y_vetrex_1 )         marching_cubes_key |= (1 << 1);
                        if( min_y + iter_k * marching_cubes_cube_length <= y_vetrex_2 )         marching_cubes_key |= (1 << 2);
                        if( min_y + iter_k * marching_cubes_cube_length <= y_vetrex_3 )         marching_cubes_key |= (1 << 3);
                        if( min_y + ( iter_k + 1 ) * marching_cubes_cube_length <= y_vetrex_0 ) marching_cubes_key |= (1 << 4);
                        if( min_y + ( iter_k + 1 ) * marching_cubes_cube_length <= y_vetrex_1 ) marching_cubes_key |= (1 << 5);
                        if( min_y + ( iter_k + 1 ) * marching_cubes_cube_length <= y_vetrex_2 ) marching_cubes_key |= (1 << 6);
                        if( min_y + ( iter_k + 1 ) * marching_cubes_cube_length <= y_vetrex_3 ) marching_cubes_key |= (1 << 7);
                        
                        int edges_activated = this->edges_marching_cubes[marching_cubes_key];
                        int number_of_edges_activated = 0;
                        int starting_index_of_index_buffer = temp_vertex_buffer.size() / 3;
                        
                        // int number_of_edges_activated = std::popcount(edges_activated);
                        for(int iter_l = 0; iter_l < 12 ; iter_l++ ){
                            if( edges_activated & (1 << iter_l) ){
                                vertex_index_to_edge_map[ number_of_edges_activated ] = iter_l;
                                edge_to_vertex_index_map[ iter_l ] = number_of_edges_activated;
                                number_of_edges_activated++;
                            }
                        }
                        for(int iter_l = 0; iter_l < number_of_edges_activated ; iter_l++){
                            const int edge_activated = vertex_index_to_edge_map[ iter_l ];
                            
                            switch(edge_activated){
                                case 0:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length / 2 );
                                    temp_vertex_buffer.push_back( min_y + iter_k * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter );
                                    break;
                                case 1:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( min_y + iter_k * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length / 2 );
                                    break;
                                case 2:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length / 2 );
                                    temp_vertex_buffer.push_back( min_y + iter_k * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length );
                                    break;
                                case 3:
                                    temp_vertex_buffer.push_back( x_this_iter );
                                    temp_vertex_buffer.push_back( min_y + iter_k * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length / 2 );
                                    break;
                                
                                case 4:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length / 2 );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 1 ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter );
                                    break;
                                case 5:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 1 ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length / 2 );
                                    break;
                                case 6:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length / 2 );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 1 ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length );
                                    break;
                                case 7:
                                    temp_vertex_buffer.push_back( x_this_iter );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 1 ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length / 2 );
                                    break;
                                
                                case 8:
                                    temp_vertex_buffer.push_back( x_this_iter );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 0.5f ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter );
                                    break;
                                case 9:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 0.5f ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter );
                                    break;
                                case 10:
                                    temp_vertex_buffer.push_back( x_this_iter + marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 0.5f ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length );
                                    break;
                                case 11:
                                    temp_vertex_buffer.push_back( x_this_iter );
                                    temp_vertex_buffer.push_back( min_y + ( iter_k + 0.5f ) * marching_cubes_cube_length );
                                    temp_vertex_buffer.push_back( z_this_iter + marching_cubes_cube_length );
                                    break;
                                default:
                                    __builtin_trap(); //Posix Function. Comment this is using windows
                                    break;

                            }
                            
                        }

                        for(int iter_l = 0 ; true ; iter_l++){
                            if(Scene::triangulation[ marching_cubes_key ][ 3 * iter_l ] == -1)
                                break; 
                            temp_index_buffer.push_back( starting_index_of_index_buffer + edge_to_vertex_index_map[ Scene::triangulation[ marching_cubes_key ][ 3 * iter_l     ] ] );
                            temp_index_buffer.push_back( starting_index_of_index_buffer + edge_to_vertex_index_map[ Scene::triangulation[ marching_cubes_key ][ 3 * iter_l + 1 ] ] );
                            temp_index_buffer.push_back( starting_index_of_index_buffer + edge_to_vertex_index_map[ Scene::triangulation[ marching_cubes_key ][ 3 * iter_l + 2 ] ] );
                        }
                    }

                }
            }
            this->worker_thread_update_state(my_thread_id, temp_vertex_buffer, temp_index_buffer);
            std::cout<<"Worker thread "<< my_thread_id << " has ended working"<<std::endl;

        }

    }

}

void Scene::worker_thread_update_state(int its_thread_id,const std::vector<float> &temp_vertex_buffer, const std::vector<unsigned int> &temp_index_buffer){
    if ( this->vertex_buffers_per_thread.at(its_thread_id) ) 
        delete[] this->vertex_buffers_per_thread.at(its_thread_id);
    
    this->vertices_per_thread.at(its_thread_id) = temp_vertex_buffer.size();
    std::cout<<its_thread_id<<'\t'<<this->vertices_per_thread.at(its_thread_id)/3<<std::endl;
    this->vertex_buffers_per_thread.at(its_thread_id) = new float[temp_vertex_buffer.size()];
    std::copy( temp_vertex_buffer.data(), temp_vertex_buffer.data() + temp_vertex_buffer.size(), vertex_buffers_per_thread.at(its_thread_id) );

    if ( this->indexs_per_thread.at(its_thread_id) )
        delete[] this->index_buffers_per_thread.at(its_thread_id);

    this->indexs_per_thread.at(its_thread_id) = temp_index_buffer.size();
    this->index_buffers_per_thread.at(its_thread_id) = new unsigned int[temp_index_buffer.size()];
    std::copy( temp_index_buffer.data(), temp_index_buffer.data() + temp_index_buffer.size(), this->index_buffers_per_thread.at(its_thread_id) );

    thread_done_status.at(its_thread_id) = true;
}


// Values from http://paulbourke.net/geometry/polygonise/

const int Scene::edges_marching_cubes[256] = {
    0x0,
    0x109,
    0x203,
    0x30a,
    0x406,
    0x50f,
    0x605,
    0x70c,
    0x80c,
    0x905,
    0xa0f,
    0xb06,
    0xc0a,
    0xd03,
    0xe09,
    0xf00,
    0x190,
    0x99,
    0x393,
    0x29a,
    0x596,
    0x49f,
    0x795,
    0x69c,
    0x99c,
    0x895,
    0xb9f,
    0xa96,
    0xd9a,
    0xc93,
    0xf99,
    0xe90,
    0x230,
    0x339,
    0x33,
    0x13a,
    0x636,
    0x73f,
    0x435,
    0x53c,
    0xa3c,
    0xb35,
    0x83f,
    0x936,
    0xe3a,
    0xf33,
    0xc39,
    0xd30,
    0x3a0,
    0x2a9,
    0x1a3,
    0xaa,
    0x7a6,
    0x6af,
    0x5a5,
    0x4ac,
    0xbac,
    0xaa5,
    0x9af,
    0x8a6,
    0xfaa,
    0xea3,
    0xda9,
    0xca0,
    0x460,
    0x569,
    0x663,
    0x76a,
    0x66,
    0x16f,
    0x265,
    0x36c,
    0xc6c,
    0xd65,
    0xe6f,
    0xf66,
    0x86a,
    0x963,
    0xa69,
    0xb60,
    0x5f0,
    0x4f9,
    0x7f3,
    0x6fa,
    0x1f6,
    0xff,
    0x3f5,
    0x2fc,
    0xdfc,
    0xcf5,
    0xfff,
    0xef6,
    0x9fa,
    0x8f3,
    0xbf9,
    0xaf0,
    0x650,
    0x759,
    0x453,
    0x55a,
    0x256,
    0x35f,
    0x55,
    0x15c,
    0xe5c,
    0xf55,
    0xc5f,
    0xd56,
    0xa5a,
    0xb53,
    0x859,
    0x950,
    0x7c0,
    0x6c9,
    0x5c3,
    0x4ca,
    0x3c6,
    0x2cf,
    0x1c5,
    0xcc,
    0xfcc,
    0xec5,
    0xdcf,
    0xcc6,
    0xbca,
    0xac3,
    0x9c9,
    0x8c0,
    0x8c0,
    0x9c9,
    0xac3,
    0xbca,
    0xcc6,
    0xdcf,
    0xec5,
    0xfcc,
    0xcc,
    0x1c5,
    0x2cf,
    0x3c6,
    0x4ca,
    0x5c3,
    0x6c9,
    0x7c0,
    0x950,
    0x859,
    0xb53,
    0xa5a,
    0xd56,
    0xc5f,
    0xf55,
    0xe5c,
    0x15c,
    0x55,
    0x35f,
    0x256,
    0x55a,
    0x453,
    0x759,
    0x650,
    0xaf0,
    0xbf9,
    0x8f3,
    0x9fa,
    0xef6,
    0xfff,
    0xcf5,
    0xdfc,
    0x2fc,
    0x3f5,
    0xff,
    0x1f6,
    0x6fa,
    0x7f3,
    0x4f9,
    0x5f0,
    0xb60,
    0xa69,
    0x963,
    0x86a,
    0xf66,
    0xe6f,
    0xd65,
    0xc6c,
    0x36c,
    0x265,
    0x16f,
    0x66,
    0x76a,
    0x663,
    0x569,
    0x460,
    0xca0,
    0xda9,
    0xea3,
    0xfaa,
    0x8a6,
    0x9af,
    0xaa5,
    0xbac,
    0x4ac,
    0x5a5,
    0x6af,
    0x7a6,
    0xaa,
    0x1a3,
    0x2a9,
    0x3a0,
    0xd30,
    0xc39,
    0xf33,
    0xe3a,
    0x936,
    0x83f,
    0xb35,
    0xa3c,
    0x53c,
    0x435,
    0x73f,
    0x636,
    0x13a,
    0x33,
    0x339,
    0x230,
    0xe90,
    0xf99,
    0xc93,
    0xd9a,
    0xa96,
    0xb9f,
    0x895,
    0x99c,
    0x69c,
    0x795,
    0x49f,
    0x596,
    0x29a,
    0x393,
    0x99,
    0x190,
    0xf00,
    0xe09,
    0xd03,
    0xc0a,
    0xb06,
    0xa0f,
    0x905,
    0x80c,
    0x70c,
    0x605,
    0x50f,
    0x406,
    0x30a,
    0x203,
    0x109,
    0x0
};

const int Scene::triangulation[256][16] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
    { 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1 },
    { 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1 },
    { 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
    { 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 },
    { 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1 },
    { 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1 },
    { 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
    { 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1 },
    { 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1 },
    { 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
    { 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
    { 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1 },
    { 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1 },
    { 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1 },
    { 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 },
    { 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1 },
    { 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
    { 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
    { 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
    { 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1 },
    { 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1 },
    { 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
    { 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
    { 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1 },
    { 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1 },
    { 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
    { 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1 },
    { 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1 },
    { 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
    { 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
    { 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1 },
    { 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1 },
    { 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
    { 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1 },
    { 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1 },
    { 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1 },
    { 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1 },
    { 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 },
    { 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
    { 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1 },
    { 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
    { 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
    { 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1 },
    { 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1 },
    { 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1 },
    { 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1 },
    { 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1 },
    { 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 },
    { 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 },
    { 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1 },
    { 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1 },
    { 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1 },
    { 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1 },
    { 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1 },
    { 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1 },
    { 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1 },
    { 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1 },
    { 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1 },
    { 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1 },
    { 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1 },
    { 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1 },
    { 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1 },
    { 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1 },
    { 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1 },
    { 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1 },
    { 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1 },
    { 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1 },
    { 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1 },
    { 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 },
    { 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1 },
    { 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1 },
    { 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1 },
    { 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1 },
    { 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1 },
    { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 },
    { 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1 },
    { 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1 },
    { 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1 },
    { 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1 },
    { 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1 },
    { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1 },
    { 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1 },
    { 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1 },
    { 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1 },
    { 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1 },
    { 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1 },
    { 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1 },
    { 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1 },
    { 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1 },
    { 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1 },
    { 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1 },
    { 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 },
    { 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1 },
    { 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1 },
    { 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1 },
    { 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1 },
    { 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1 },
    { 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1 },
    { 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1 },
    { 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1 },
    { 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1 },
    { 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 },
    { 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1 },
    { 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1 },
    { 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1 },
    { 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1 },
    { 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1 },
    { 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
};

const int Scene::cornerIndexAFromEdge[12] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    0,
    1,
    2,
    3
};

const int Scene::cornerIndexBFromEdge[12] = {
    1,
    2,
    3,
    0,
    5,
    6,
    7,
    4,
    4,
    5,
    6,
    7
};

const int Scene::number_of_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1; //hardware_concurrency can fail and give 0
