#pragma once
#include <vector>
#include <random>
namespace Randomness{
class Random_t{
private:
    const unsigned seed = 69420;
    // const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rndm_nmbr_gnrt;
    const float min = std::mt19937::min();
    const float max = std::mt19937::max();
    Random_t():rndm_nmbr_gnrt(seed){};
public:
    static Random_t Random;
    float rand(){
        return (static_cast<float>(static_cast<int32_t>(rndm_nmbr_gnrt())) - min) / (max - min);
    }
    float rand_f(float l, float r){
        return l + (r-l)*(rand() + 1) / 2;
    }
    float rand_f(float f){
        return rand_f(0, f);
    }
    int rand(int l, int r){
        int diff = r - l;
        int rndm_nmbr = abs(static_cast<int>(rndm_nmbr_gnrt()));
        return l + ( rndm_nmbr % diff );
    }
    int rand(int n){
        return rand(0, n);
    }
    void hash_and_seed(int a, int b){
        /*Szudzik's function*/
        int A = a >= 0 ? 2 * a : -2 * a - 1;
        int B = b >= 0 ? 2 * b : -2 * b - 1;
        int C = (A >= B ? A * A + A + B : A + B * B) / 2;
        int seed = ((a < 0 && b < 0) || (a >= 0 && b >= 0)) ? C : -C - 1;
        rndm_nmbr_gnrt.seed(seed);
    }
};
} 


