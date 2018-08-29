# Software Renderer Project
### Raytracer and Rasterizer developed in pairs for the COMS30115 Computer Graphics unit  at the University of Bristol

This set of programs implements two methods of 3D rendering: raytracing and rasterization. The code is written using OpenMP so that all available processor cores can be used to generate the final image.

Our raytracer implements global illumination with importance sampling to accurately simulate bounced light and area lights.

## Running the programs

Download the latest pre-compiled version from the [releases](https://github.com/Nick-Pearson/SoftwareRenderer/releases) page (if your platform is unavailable you may have to compile from source - see bellow).

Extract the .zip files to any location and run the executable files. If you are on Linux you must also ensure that you have the SDL2 library installed.

Please allow at least 10 minutes for the raytracer to generate a render with global illumination (depending on CPU and resolution).

## Compiling from Source

This repository uses CMake to allow the code to be compiled on multiple different platforms. To compile you need an up to date version of CMake ([https://cmake.org/download/](https://cmake.org/download/)) and SDL2 ([https://wiki.libsdl.org/Installation](https://wiki.libsdl.org/Installation)).

Linux users must ensure they have installed the `g++` c++ compiler and `gnu make` utility. Windows users must ensure they have a version of Visual Studio installed.


1. Open a command line at the root of this repository and run the command `cmake .`
2. CMake will now generate a build file for your platform (a Makefile for Linux or a Visual Studio solution for windows) which you can use to compile the project.

### Compilation Options

You can pass the following options into CMake:
* `-DUSE_SDL=OFF`: Compiles the programs without using the SDL library. This runs the renderers without a realtime display but does allow them to be run headerless (e.g. for use on a render farm)
* `-DGI=OFF`: Compiles the raytracer without global illumination which yields significantly faster render times without any bounce or area lighting

The layout of geometry is set in the **main.cpp** file. Geometry can only be changed by modifying this file, however it does support importing of **.obj** and **.mtl** files.

## Acknowledgements

The Mountain cubemaps and floor textures in this repository are download from Open Game Art and are licenced under CC-BY-3.0 [https://opengameart.org/content/mountain-skyboxes](https://opengameart.org/content/mountain-skyboxes)
[https://opengameart.org/content/metalstone-textures](https://opengameart.org/content/metalstone-textures)

The bunny mesh is from the Stanford 3D scan library [http://graphics.stanford.edu/data/3Dscanrep/](http://graphics.stanford.edu/data/3Dscanrep/)
