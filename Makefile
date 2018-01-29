########
#   Directories
S_DIR=src
B_DIR=build

########
#   Output
EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-c -pipe -Wall -Wno-switch -ggdb -g3 -std=c++11 -O3
LN_OPTS=
CC=g++

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
	$(CC) $(LN_OPTS) -o $@ $(OBJS) $(SDL_LDFLAGS)

# TODO: fill in when required
$(B_DIR)/rasterizer:

########
#   Code to compile each cpp file
$(B_DIR)/%.o : %.cpp $(B_DIR)/%.d
	mkdir -p $(dir $@)
	$(CC) $(CC_OPTS) -o $@ $< $(SDL_CFLAGS) $(GLM_CFLAGS)

# compilation of the .d dependancy files so we dont have to worry about headers
$(B_DIR)/%.d: %.cpp
	mkdir -p $(dir $@)
	printf $(dir $@) > $@
	$(CC) $(CC_OPTS) -MM -MG $*.cpp >> $@

clean:
	rm -rf $(B_DIR)

#-include $(SRCS:%.cpp=$(B_DIR)/%.d)
