## Computer Graphics Report
Nick Pearson (np14842) & Ben Allen (ba14800)

In order to make best use of optimisation and extensions we decided to implement the raytracer and rasterizer in the same code solution. This is achieved using compile flags set in the makefile
Over the course of the project we also removed the dependancy on SDL, this was so we could run the raytracer without having to supervise a render.

#### Requirements
- OpenMP

### 1. Raytracer
This is our final raytracer image (1024x1024, 10,000 samples per pixel). To compile and run use `make raytracer && ./build/raytracer`.

![alt text](images/final-raytracer.jpg)

### 1.1 Extensions

#### Global illumination
We implemented path tracing global illumination, we then extended this to include importance sampling for our specular, reflective and refractive materials.
The results of these can be seen in the final image particularly in the floor material which is normal mapped and has a specular material.

#### Depth of field
TODO

### Implicit surfaces
Our solution includes implicit surfaces using an abtract base class, we implemented a sphere surface but this systemc could be used to create any surface with a Ray intersection function

#### Object Loader
We implemented a loader for ".obj" files which includes loading of vertex positions, triangles, texture coordinates and material definitions
This can be seen from the rabbit that is loaded in.
 
#### KD-Trees
TODO

### Other Optimisations
We heavily optimised the raytracer code including
* Backface culling
* Cramer's Rule in Triangle solver
* Bounding box checks on meshes before the triangle intersections are run

#### OpenMP + Blue crystal
OpenMP is used to concurrently perform calculations. This was found to a significant speed boost. In the global illumination render we are able to run samples accross as many threads as are availible.
Due to our work to remove the dependancy on SDL our raytracer can be run on Blue Crystal which was used to produce our final high resolution and sample count render.

### 2. Rasterizer
This is our final rasterizer image. To compile and run use `make rasterizer && ./build/raytracer`.

![Load in rasterizer final pic here](images/final-rasterizer.jpg)

### 2.1 Extensions
#### Shadows
We implemented shadow maps for our point light source. As the point light is omnidirectional this was combined with our work on cubemaps to generate a 
shadow cubemap for the point light so that it can case shadows in any direction. We also support many light sources using forward rendering
We generate soft shadows by using the percentage closer filtering method sampling the shadow map around a pixel to determine if it is in shadow

#### Texture mapping
We implemented texture coordinates on our verticies and used our existing interpolation code to sample across the triangle.
We implemented our vertex properties as a struct with custom operators so these coordinates could be interpolated with minimal code changes

#### Normal Mapping
We can load and read tangent space normal maps. We generate triangle tangent and bitangent vectors when calculating material properties and use these to modify the triangle normal for shading

#### Clipping
Our project uses some extended clipping, unfortunately it does not support full advanced clipping. Our renderer can clip triangles against the x, y and w planes but not the z plane

#### FXAA
We make use of Fast Anti Aliasing to smooth sharp edges. With FXAA this is done in screen space to reduce computation time. Here is a before & after.

![alt text](images/fxaa-before.png)
![alt text](images/fxaa-after.png)

This is implemented in both the rasterizer and raytracer.