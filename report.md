## Computer Graphics Report
Nick Pearson (np) & Ben Allen (ba14800)

#### Requirements
- Open CV 2
- OpenMP

### 1. Raytracer
This is our final raytracer image. To compile and run use `make raytracer && ./build/raytracer`.

![alt text](images/final-raytracer.jpg)

### 1.1 Extensions

#### Global illumination
TODO

#### FXAA
We make use of Fast Anti Aliasing to smooth sharp edges. With FXAA this is done in screen space to reduce computation time. Here is a before & after.

![alt text](images/fxaa-before.png)
![alt text](images/fxaa-after.png)


#### KD-Trees
TODO

#### Depth of field
TODO

#### OpenMP + Blue crystal
OpenMP is used to concurrently perform calculations. This was found to a significant speed boost. We are then able to run the raytracer on bluecrystal for optimum speeds.

#### Object Loader
Object files can be loaded into the scene, using the left and right hand coordinate systems. Material definitions and vertex positions, normals and textures can also be loaded.
 This can be seen from the rabbit that is loaded in.


 ### 2. Rasterizer
 This is our final rasterizer image. To compile and run use `make rasterizer && ./build/raytracer`.

 ![Load in rasterizer final pic here](images/final-rasterizer.jpg)

### 2.1 Extensions
#### Shadows
Description needed

#### Texture mapping
TODO

#### Normal Mapping
TODO

#### Clipping
In-progress

#### Object Loader
Similar to the raytracer, object files can be loaded into the scene, using the left and right hand coordinate systems. Material definitions and vertex positions, normals and textures can also be loaded.
 This can be seen from the rabbit that is loaded in.
