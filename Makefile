########
#   Directories
S_DIR=src
B_DIR=build
R_DIR=resources

########
#   Output
EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-c -pipe -Wno-switch -ggdb -g3 -std=c++11 -O3 -g
LN_OPTS=-g
CC=g++
OS = $(shell uname)

#######
# OpenMP Library
ifeq ($(OS), Linux)
  OPENMP_LIB = -fopenmp
else
  OPENMP_LIB = -qopenmp -Wl,-rpath,/opt/intel/compilers_and_libraries_2018.0.104/mac/compiler/lib
endif

########
#       SDL options
ifdef __no_sdl
  SDL_CFLAGS := -D USE_SDL=0
  SDL_LDFLAGS := -D USE_SDL=0
else
  SDL_CFLAGS := -D USE_SDL=1 $(shell sdl2-config --cflags)
  SDL_LDFLAGS := -D USE_SDL=1 $(shell sdl2-config --libs)
endif

GLM_CFLAGS := -I./libs/glm/ -I./libs/stb

#######
# File lists
SRCS = $(shell find $(S_DIR) -name *.cpp)
RAY_OBJS = $(SRCS:%.cpp=$(B_DIR)/src_ray/%.o)
RAS_OBJS = $(SRCS:%.cpp=$(B_DIR)/src_ras/%.o)

########
#   Some phony targets as shortcuts
DEFAULT = rasterizer
default: $(DEFAULT)
all: raytracer rasterizer
run: $(DEFAULT)
	./$(B_DIR)/$(DEFAULT)

raytracer: $(B_DIR)/raytracer
rasterizer: $(B_DIR)/rasterizer

# linking code for both our executables
$(B_DIR)/raytracer: $(RAY_OBJS) $(B_DIR)/$(R_DIR)
	$(CC) $(LN_OPTS) $(OPENMP_LIB) -o $@ $(RAY_OBJS) $(SDL_LDFLAGS)

$(B_DIR)/rasterizer: $(RAS_OBJS) $(B_DIR)/$(R_DIR)
	$(CC) $(LN_OPTS) $(OPENMP_LIB) -o $@ $(RAS_OBJS) $(SDL_LDFLAGS)

## Resources
$(B_DIR)/$(R_DIR):
	ln -s ../$(R_DIR) $(B_DIR)/$(R_DIR)

########
#   Code to compile each cpp file
$(B_DIR)/src_ray/%.o : %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CC_OPTS) $(OPENMP_LIB) -o $@ $< $(SDL_CFLAGS) $(GLM_CFLAGS) -D RAYTRACER=1 -D RASTERIZER=0

$(B_DIR)/src_ras/%.o : %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CC_OPTS) $(OPENMP_LIB) -o $@ $< $(SDL_CFLAGS) $(GLM_CFLAGS) -D RAYTRACER=0 -D RASTERIZER=1

clean:
	rm -rf $(B_DIR)
