<h1>Flight Simulator </h1>
<br><br>
<img alt="preview" src="ScreenShots/Game.gif" >

<img alt="preview" src="ScreenShots/Game.png" >

The terrain is generated using Perlin Noise and 3 D perlin noise is used to generate dynamic flow field that dictate the movement and rotation of the clouds. 

To use
<br>
git pull this project. 
<br>
Install glfw3 on your system
<br>
download glm from github and place the glm library in the /usr/include directory.
<br>
Install make and just use the make command.
<br><br>
It won't run on windows ( sorry :| ). A lot of files have their positions hard coded relative to the executable. To use on windows you will have to find the names of all shaders and assets files and folders in the code and rename them with forward slashes. The clouds and trees constructor will also have to changed to use forward slash. And the `__builtin_trap` defined in the assert macro will have to be changed to something that windows supports. 
Then use visual studios to build the project after installing glfw3, GLEW and glm. 

<h4>Credits</h4>
Thanks to Yan Chernikov for their amazing OpenGL Series
<br>

Thanks to Niels Lohmann for the [JSON C++ Library](https://github.com/nlohmann/json)
<br>

Thanks to Ryo Suzuki for their C++ [Perlin Noise](https://github.com/Reputeless/PerlinNoise)
<br>

Thanks to Ashima Arts for their glsl implementations of [noises](https://github.com/ashima/webgl-noise) 

<h4>Credits for the assets </h4>

Airplane by Poly by Google [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/8VysVKMXN2J)

Airplane by Poly by Google [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/8ciDd9k8wha)


Aeroplane by Gilang Romadhan [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/9VeIc0cybp4)


Grass by hat_my_guy (https://poly.pizza/m/9S0fmIfGPO)

grass blades by Tiff Eidmann [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/7jaHZEe1exG)

Grass #1 by Tomáš Bayer [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/00rprwmzLKP)

Cumulus Clouds 5 by S. Paul Michael [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/25RmW99gwuv)

Cumulus Clouds 2 by S. Paul Michael [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/5ckRCisrnXh)    

Clouds by Jarlan Perez [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/b3Kia9N2fS2)

Cumuls Clouds 3 by S. Paul Michael [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/3wzu2FRYXUi)

Cloud by Poly by Google [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/44cGXp6_8WD)

Cloud by Quaternius (https://poly.pizza/m/KdFNOVn1Gf)

Clouds by Poly by Google [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/5vL346OfNST)

Cloud by Poly by Google [CC-BY](https://creativecommons.org/licenses/by/3.0/) via [Poly Pizza](https://poly.pizza/m/aQl2tRgDupm)

Cloud by [Quaternius](https://poly.pizza/m/P1cMV8qtN2)

Cloud by [Quaternius](https://poly.pizza/m/pjiBiLJwAl)

Cloud by [Quaternius](https://poly.pizza/m/F6DzCxDz6I)

Cloud by jeremy [CC-BY] via [Poly Pizza]