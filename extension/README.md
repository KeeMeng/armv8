# Part 4 - Extension Project: Arcade Anthology

## How To Use the Engine

1. Create your game files
	- First, create two files in the games folder - gameName.h and gameName.c where gameName is the name of your game.

2. Implement the state data structure
   - Declare the state data structure in gameName.h. This should contain all of the textures and game objects that your game uses. This will be passed to all of the game functions.

3. Declare the functions
	- There are 4 functions that you need to declare in gameName.h:
	```
	void initGameName(Game * game, void ** state);
	// The first argument is the game engine structure.
	// The second argument is a pointer to the game structure - this is the structure you created in the previous section. 
	// You must dynamically allocate this using `*state = malloc`(sizeof(Your structure goes here...));

	void updateGameName(Game * game, void * state);
	// This handles all game logic - moving game objects, physics, etc.

	void renderGameName(Game * game, void * state);
	// This draws all game objects and text using functions such as renderSprite and renderString.

	void exitGameName(Game * game, void * state);
	// This the structure declared in section 2 and pointed to by state. It should free the textures loaded in initialise.
	```
	- At this point - try and merge back to the extension_development branch then let me (Will) know - before you can run the program, I have to add some code to get the engine to recognise your game state.

1. Loading media:
	- You may want to load media in the initGameName function. You can do this using the loadTexture function: `bool loadTexture(Texture * tex, const char * path);`

	- The first argument is a pointer to the texture that you want to load into.

	- I find it helpful to package all the texture loading code up into one function and execute it in initGameName.

2. Getting input:
	- To get input, usually during the update function, you can check the game engine struct, passed into the functions as Game * game. 
	- You can check if a key was pressed and check how long it has been held down for:
	```
	if (game->keysDown['A']) {
		// move left...
	} else if(game->keysDown['D']) {
		// move right ...
	} else {
		// stationary ...
	}

	if (game->keyTime['A'] > 10.0) {
		// A has been pressed down for
		// more than 10 seconds
	}
	```

3. Texture Struct
	- The texture struct has the following format:

	```
	typedef struct {
	   int width;
	   int height;
	   int channels;
	   Pixel * pixels;
	   unsigned int textureId;
	} Texture;
	```

	The pixel structure stores 4 bytes -
	red, green, blue and alpha.

	```
	typedef struct {
	   uint8_t red;
	   uint8_t green;
	   uint8_t blue;
	   uint8_t alpha;
	} Pixel;
	```

	This data is all filled out by the
	loadTexture function, detailed above.

4. Sprite Struct
	- The sprite structure is used to apply a
	transformation to the texture.

	```
	typedef struct {
	   Vec2f position;
	   Vec2f size;
	   float angle;
	   Vec2f origin;
	   Rectanglef texCoords;
	   Texture * texture;
	} Sprite;
	```

	We have to initialise a sprite before we
	can draw it. We do this by setting its
	fields:
	- texture - this is a pointer to the texture to be rendered.

	- size - this is the size of the image to be drawn to the screen in pixels.

	- angle - this is the angle of rotation around the origin. This angle is anticlockwise.

	- origin - this is anchor point of the sprite. When we set the position to draw at, the position is the point at which we draw the
	origin.
	
		E.g. if we have a 64x32 sprite and we set the
		origin to (32,16) then the sprite will be
		centred, such that all transformations occur
		around the centre (point (32, 16) on all the
		sprites).
	
	- tex coords - this is a rectangle that carves out a region of the texture to be drawn. We use this to render animations from a texture - by carving out each frame one after the other.

5. Rendering Sprites
	- After we have initialised a sprite, we can draw it using the renderSprite function: `void renderSprite(Renderer * r, Sprite * spr);`

	- We pass in the renderer - for your games, you'll want to pass in game->renderer.
	- We also pass in the pointer to the sprite.

6. Rendering Text
    - There is no need to initialise the font - this is done by
	the game engine. You can render text using the renderString function: `void renderString(Renderer * r, const char * str, double x, double y, double width, double height);`
	- The width and height are in pixels - the dimensions of each character in the string.

7.  Delta Time
	- We want the gameplay to be framerate independent.
	- I.e. the player should move across the screen at the same speed regardless of the framerate of the program.
	- To accomplish this we multiply all movements by the time taken for the frame to execute. This time is called delta time and can be retrieved using the following game->deltaTime
	- E.g. if we want to move the player by 500 pixels per
	second, we would write:
	```
     player->velocity.x = 500;
     player->position.x += player->velocity.x * game->delta
     ```

	- Please look at the files minotaurs.h and minotaurs.c as an additional example.

## Additional Documentation
For the extension of our project, we will build a collection of arcade-style games. Players will be able to choose one of four games to play. All the games are high-score oriented, and results are saved to a leaderboard stored on disk.

Under the hood, the project will use OpenGL, via the GLEW and GLFW libraries for obtaining the OpenGL function addresses and for handling windows and events respectively.

We will embed the sprite files as bitmaps in the code since this makes it easier to package the game up into an executable. We may do the same for the shaders as well.

The project will be comprised of the following modules:

- window:
  - Provides data structures and functions - for creating windows and handling events.

- graphics:
	- Provides data structures and functions for rendering 2d sprites and geometry. It also provides functions for initialising public functions

- main
	- Includes the entry point of the program. Loads resources needed
