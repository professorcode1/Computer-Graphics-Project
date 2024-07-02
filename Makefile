EXE = web/index.js
CHERNO_LIB = OpenGL
MY_CODE = Source
SOURCES = FlightSimulator.cpp
GLM_LIB = glm
SOURCES += $(wildcard $(CHERNO_LIB)/*.cpp)
SOURCES += $(wildcard $(GLM_LIB)/*.cpp)
SOURCES += $(wildcard $(MY_CODE)/*.cpp)

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

# CXXFLAGS += -I/home/raghavk/glm/glm
CXXFLAGS += -g -Wall -Wformat
LIBS = -lGLEW

ECHO_MESSAGE = "Emscripten"
CC = emcc
CXX = em++
CXXFLAGS += -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s ASSERTIONS=1  --preload-file assets --preload-file shaders --preload-file parameters.json
LIBS += -lGL -s USE_WEBGL2=1

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(CHERNO_LIB)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(MY_CODE)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)

clean_obj:
	rm -f  $(OBJS)
