_Done:_
* Object Loader
* Bounding box optimisation for raytracer (This can be improved by using a bounding box hierarchy or KD trees)
* Specular materials
* Simple multithreading

_In Progress_:
* Full clipping in rasterizer
* Shadow mapping in rasterizer
* FXAA anti-aliasing
* Vertex shading

_Planned:_
* Soft shadows in raytracer (by using multiple shadow rays)
* Reflective & Refractive materials in raytracer
* Proper global illumination (current implementation is a bit broken)
* Refactor to so that we can compile and run on blue crystal to produce our final results
* Textures / Normal maps

_Other ideas:_
* Frustrum culling (rasteriser)
* Porting some code to the GPU (preferably using open CL as this allows us direct control of memory movements)
* Rasteriser soft shadows (by using percentage closer / screen space gaussian techniques)
* Rasteriser screen space reflections
* Rasteriser depth buffer fog
* Screen space ambient occlusion [Example for Open GL](http://ogldev.atspace.co.uk/www/tutorial45/tutorial45.html)
* Environment maps as a source for lighting (raytracer)
* Rasteriser Lightmaps? (this seems like an easy extension if we have textures implemented and could optimise the rasteriser a lot / allow us to add better effects)
* Other types of light: directional, spot etc.
* Procedural skybox [Scratchapixel Tutorial](https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky)