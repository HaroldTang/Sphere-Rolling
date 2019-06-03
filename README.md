# Sphere Rolling Project
Interactive computer graphic project. Use OpenGL and shader programming in lighting, texture mapping, reflection, particle effect.
For this project, the sphere is imported from file and can start from one corner of the plane. 
- When click on the 'B' button on the keyboard, make the sphere rolling on the surface. 
- With the left mouse clicking, stop the sphere from rolling.
- With the right mouse menu options:
  - Create the shadow effect of the sphere (based on homogeneous coordinate system - shadow matrix)
  - Texture mapping on the sphere and the floor (use texture coordinate), also can click on keyboard 'U', 'A', 'V', 'T' to change the effect of mapping.
  - Rendering using lighting setting (normal vector) or just use original given color (4-dimensions vector).
  - Lighting setting (point source, spotlight or distant light)
  - Change the solid sphere setting to get a linear sphere and a linear ground.
  - Create a fog effect on the sphere and floor. Can change the setting of the fog (linear fog, exponential fog, etc)
  - Create a firework effect with particle doing free fall movement (random velocity and random color to create beautiful firework!)
- Click on the 'L' button on the keyboard to create a lattice effect on the sphere. 

## Bresenham’s scan-conversion algorithm for a Circle
Use Bresenham’s scan-conversion algorithm to derive an algorithm for OpenGL to draw a circle. 
Use a variable binding with `idle()` function to create a movement of circles.
