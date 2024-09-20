# libGLRender

### A generic and portable OpenGL 4.5+ rendering engine and API

Provides the following classes:
* Renderer - The rendering engine
* Shader - OpenGL vert/frag or compute shader
* Mesh - OpenGL geometry
* Texture - OpenGL texture
* Framebuffer - OpenGL framebuffer object
* PostPass - Postprocessing step
* PostStack - Ordered collection of PostPass steps
* Image - On-CPU editable image
* Atlas - OpenGL texture made from smaller images stitched together
* Color - An intermediary color representation with conversions


Windowing libraries like SDL2 can be used to load OpenGL's functions by passing their load proc function pointer to the constructor of Renderer.

This library uses GLAD to load OpenGL functions.  There is no option to set up libGLRender in an existing context.
