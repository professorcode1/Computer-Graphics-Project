# /bin/sh
SourceFiles=$(ls Source/*.cpp)
OpenGLFiles=$(ls OpenGL/*.cpp)
GLMFIles=$(ls OpenGL/*.cpp)
# emcc --bind -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s \
# 'EXPORT_NAME="FlightSimulatorModule"' -s USE_WEBGL2=1 -I/usr/include -o \
# web/index.js main.cpp ${SourceFiles} ${OpenGLFiles} -lGL -lGLU -lglut
# emcc --bind -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s 'EXPORT_NAME="FlightSimulatorModule"' -I/usr/include -o web/index.js FlightSimulator.cpp -lGL -lGLU -lglut
emcc FlightSimulator.cpp  -I/usr/include -lGL -lGLU -lglut  -s WASM=1 -s LEGACY_GL_EMULATION=1 -O3  -o web/index.js --preload-file assets
