#pragma once
#include <GL/glut.h>
#include <emscripten.h>
#include <stdio.h>
#include "IncGLM.hpp"
#include "OpenGL/Skybox.hpp"
#include "Source/plane.h"
#include "FOSS_Code/json.hpp"
#include "Source/Grid.h"
#include <cstring>
using json = nlohmann::json;

constexpr int KEY_VALUE_OUTSIDE_GLUT_RANGE = 1000;
char const * const RenderingParameterJSON = R"(
{
    "sky color RGB" : [135,206,235],
    "fog density" : 0.00025,
    "terrain parameters":{
        "divisions" : 10, 
        "wave numbers active" : [0,1,2,3,4,5,6,7,8,9,10],
        "rotation angle fractal ground" : 36.87,
        "output_increase_fctr_" : 3.0,
        "input_shrink_fctr_": 3.0,
        "length of one side":20.0,
        "lacunarity" : 2.0,
        "persistance" : 3.1,
        "Mountain Scale Factor" : 50
    },
    "plane parameters":{
        "plane":"1",
        "Camera Beind Distance" : 20.0,
        "Camera Up Distance" : 3.50,
        "Camera ViewPoint Distance" : 1.0,
        "Plane Speed" : 5      
    },
    "cloud parameters":{
        "distance above terrain" : 350,
        "cloud per division" : 50,
        "scale" : 0.4,
        "input shrink factor" : 500.0,
        "time shrink factor" : 600.0,
        "velocity" : 0.15,
        "rotational velocity degree" : 0.10
    },
    "tress parameters":{
        "tress per division" : 4,
        "tress scale" : 100.0
    },
    "sun parameters":{
        "direction vector":[ -0.624695 , 0.468521 , -0.624695 ]
    },
    "camera":{
        "FOV": 45.0,
        "Near Plane" : 0.1,
        "Far Plane" : 10000.0
    },
    "planes":{
        "1":{
            "Plane OBJ file" : "assets/Planes/Aeroplane/Plane.obj",
            "Plane Texuture file" : "assets/Planes/Aeroplane/Color plane map.png",
            "Plane Scale" : 1.0,
            "rotation angles":[ 0.0 , 0.0 , 0.0 ],
            "rotation axises":[ "y" , "z" , "x" ]            
        },
        "2":{
            "Plane OBJ file" : "assets/Planes/Airplane/PUSHILIN_Plane.obj",
            "Plane Texuture file" : "assets/Planes/Airplane/PUSHILIN_PLANE.png",
            "Plane Scale" : 1.0,
            "rotation angles":[ -90.0 , -10.0 , 0.0 ],
            "rotation axises":[ "y" , "z" , "x" ]
        },
        "3":{
            "Plane OBJ file" : "assets/Planes/Airplane (1)/Airplane.obj",
            "Plane Texuture file" : "assets/Planes/Airplane (1)/Airplane Texture.png",
            "Plane Scale" : 0.0025,
            "rotation angles":[ 0.0 , 0.0 ,0.0 ],
            "rotation axises":[ "x" , "y" , "z" ]
        }

    }

}   
)";

json parameter_json = json::parse(RenderingParameterJSON);