########
#   Directories
S_DIR=src
B_DIR=build
R_DIR=resources

########
#   Output
EXEC=$(B_DIR)/$(FILE)



# default build settings
CC_OPTS=-qopenmp -c -pipe -Wall -Wno-switch -ggdb -g3 -std=c++11 -O3
LN_OPTS=-qopenmp
CC=icc
OS = $(shell uname)

#######
# OpenMP Library
ifeq ($(OS), Linux)
  OPENMP_LIB = -Wl,-rpath,/opt/intel/compilers_and_libraries_2018.1.163/linux/compiler/lib/intel64_lin
else
  OPENMP_LIB = -Wl,-rpath,/opt/intel/compilers_and_libraries_2018.0.104/mac/compiler/lib
endif

########
#       SDL options
SDL_CFLAGS := $(shell sdl2-config --cflags)
GLM_CFLAGS := -I./libs/glm/ -I./libs/stb
SDL_LDFLAGS := $(shell sdl2-config --libs)

#######
# File lists
SRCS = $(shell find $(S_DIR) -name *.cpp)
OBJS = $(SRCS:%.cpp=$(B_DIR)/%.o)

########
#   Some phony targets as shortcuts
all: raytracer rasterizer
run: raytracer
	./$(B_DIR)/raytracer

raytracer: $(B_DIR)/raytracer
rasterizer: $(B_DIR)/rasterizer

# linking code for both our executables
$(B_DIR)/raytracer: $(OBJS) $(B_DIR)/$(R_DIR)
	$(CC) $(LN_OPTS) $(OPENMP_LIB) -o $@ $(OBJS) $(SDL_LDFLAGS)

# TODO: fill in when required
$(B_DIR)/rasterizer: $(B_DIR)/$(R_DIR)

## Resources
$(B_DIR)/$(R_DIR):
	ln -s ../$(R_DIR) $(B_DIR)/$(R_DIR)

########
#   Code to compile each cpp file
# $(B_DIR)/%.d
$(B_DIR)/%.o : %.cpp
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
