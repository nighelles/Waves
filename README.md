Waves
=====

Documentation of Engine, these are the main parts as of the last time I updated this file. (8/5/2014 5:01pm)

Engine 
  -Holds all the top level code for running the game engine.
  
  TODO: GameplayController
    - Entity handling and gamelogic are currently handled in the main engine file under Engine.
    - These things should be moved to a separate system that provides functions to run the game.
    - This file might be better done in C style C++ because it doesn't intuitively map to obj style
    - and we really just need to put all the code running in Engine that doesn't directly interface
    - directly with a part of our engine somewhere it makes sense and will look clean.
    
  TODO: PhysicsController
    - Not sure if this should be under GameplayController or Engine, probably Engine
    - Should be able to register Entities or EntityModels, probably Entities
    - Deals with the physics of the engine, moving objects that collide, making players able to stand on land,
    - etc.
    - Potentially roll into Gameplay Controller?
    
  InputController
    -Deals with direct Input and gives us player input
  
  GraphicsController
    -Handles the graphics system, gives methods for drawing objects.
    -High level drawing functions, does not interface with Direct3D
    -Can Register an Entity Model as needing to be drawn every frame.
    
      RenderController
        -Deals with the lower level graphics, and polygon rendering.
        -Handles using the correct Shader, talks to Direct3D
        -Does all the fun stuff like buffers and memory handling.
        
      Shader
        -Loads the correct shader files for the object at hand.
        -Sets how a thing gets drawn, texture, procedural shader, etc.
        
      Light
        -Handles the lighting in the scene, talks to RenderController.
        
      Texture
        -Deals with importing textures
        
  NetworkSyncController
    - Holds the game state that needs to be communicated over the network.
    - Talks to the lower level NetworkController
    - TODO: delta compression
    
    NetworkController
      - Sends data over the network
      - NetworkClient/NetworkServer hold different code

Planned Entity Handling system:

EntityModel (Holds actual 3d model for an object)
  - These are used to hold the models for most of the objects in the game, owned by a Entity
  - Provides a ray-triangle intersection function, although this might be better in the PhysicsController
  - code.
  
  Terrain (EntityModel with a top/bottom)
    - These are slightly different, based on same structure, but they only really have one side
    - Provide editing functions, and render with a map.
    
    Procedural Terrain
      -Like a terrain, but designed to be procedurally calculated at runtime by shader math.
      -Right now, the water behavior is hard coded in, this could be moved to a config file.
      
Entity  (Thing that exists at a spot)
  - This is just a basic class for a thing that exists, and has a location.
  - Provides basic functionality for setting and getting rotation and location, etc.
  
  PhysicsEntity (Thing that exists at a spot and has a model)
    -Owns a EntityModel, so it can have a physical form.
    
TODO: These entities should be loaded by The gameplay module, based on config files, so that we can change the game
behavior or setup without having to recompile the whole engine.

TODO: The level editor should support adding entities, and should have player start points or whatever.
TODO: Also, the level editor needs more tools, and better interface.1 + (z + 1) + (x + 1)*MAPSIZE;