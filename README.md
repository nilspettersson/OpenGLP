# OpenGLP
This graphics library abstracts away some parts of openGl rendering.

## Core features:
#### Vao:
* Vao Class where you can create static or dynamic meshes.
#### Mesh:
* A mesh can be created by loading an obj file or by adding vertex layout, vertices and indices.
* A mesh can be used to create a vao.
#### Material:
* Material Class has a shader and the materal properties that belongs to an entity
* Material will update a list of uniforms every frame.
#### Shader:
* Shader can be loaded from disk.
#### Entity:
* Entity contains a Material.
* Entity contains a Vao.
* Entity  contains a position and scale.
* Entity can have child entites that have relative position and scale.
#### Renderer:
* The renderer can render entities and their child entites. It also handles the binding of textures, vao and more.
#### Camera:
* There is a 2d and 3d camera whith rotation and position.
