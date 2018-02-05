########
#   Directories
S_DIR=src
B_DIR=build

########
#   Output
EXEC=$(B_DIR)/$(FILE)



# default build settings
CC_OPTS=-qopenmp -c -pipe -Wall -Wno-switch -ggdb -g3 -std=c++11 -O3
LN_OPTS=-qopenmp
CC=icc


#######
# OpenMP Library
ifeq ($(OS), Linux)
  OPENMP_LIB = ""
else
  OPENMP_LIB = -Wl,-rpath,/opt/intel/compilers_and_libraries_2018.0.104/mac/compiler/lib
endif

########
#       SDL options
SDL_CFLAGS := $(shell sdl2-config --cflags)
GLM_CFLAGS := -I./libs/glm/
SDL_LDFLAGS := $(shell sdl2-config --libs)

#######
# File lists
SRCS = $(shell find $(S_DIR) -name *.cpp)
OBJS = $(SRCS:%.cpp=$(B_DIR)/%.o)

########
#   Some phony targets as shortcuts
all: raytracer rasterizer
raytracer: $(B_DIR)/raytracer
rasterizer: $(B_DIR)/rasterizer

# linking code for both our executables
$(B_DIR)/raytracer: $(OBJS)
	$(CC) $(LN_OPTS) $(OPENMP_LIB) -o $@ $(OBJS) $(SDL_LDFLAGS)

# TODO: fill in when required
$(B_DIR)/rasterizer:

########
#   Code to compile each cpp file
$(B_DIR)/%.o : %.cpp $(B_DIR)/%.d
	mkdir -p $(dir $@)
	$(CC) $(CC_OPTS) $(OPENMP_LIB) -o $@ $< $(SDL_CFLAGS) $(GLM_CFLAGS)

# compilation of the .d dependancy files so we dont have to worry about headers
$(B_DIR)/%.d: %.cpp
	mkdir -p $(dir $@)
	printf $(dir $@) > $@
	$(CC) $(CC_OPTS) $(OPENMP_LIB) -MM -MG $*.cpp >> $@

clean:
	rm -rf $(B_DIR)

# -include $(SRCS:%.cpp=$(B_DIR)/%.d)
