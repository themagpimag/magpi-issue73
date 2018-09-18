#pragma once

#include "OGL.h"
#include "Game.h"
// since we now know that our tiles all rendered together is so much faster, lets tidy up the code and put our tile set up and render into its on functional class
// this is an example of a kind of class we call a manager, it handles a group of other classes for us so we only have to use its update functions to get all the other
// smaller class functions to work. In this case it will set up and create all the tiles, and when asked will draw them

class Tiles 
{
	
// define the struct this class uses for TileVectorData	
typedef struct // A very simple structure to hold the data we need to send to the new special shader that uses uv's 
	{
		float	PositionX;    // we will load these into the shader
		float	PositionY;   // these are basially the relative offsets of the quad
		float	dummyZ;
		// now this time we send the screen pixel position this way		
		float	sPositionX;     // we will load these into the shader
		float	sPositionY;    // these are basially the relative offsets of the quad
		float	u;  		// our shader will
		float	v;
	} TileVectorData;

 const float stepX = 1.0f / 8.0f; // we could use #define but this is considered more C++
 const float stepY = 1.0f / 2.0f;
	// rework the old texture offsets using these new values	
 const GLfloat QuadTexVals[12] =    // these are the offset texture coordinates for a single cell, we need to add an offset to then to point to the correct cell for each tile.
	{
		stepX,
		0.0f,

		0.0f, 
		0.0f,		
		
		stepX,
		stepY, 	
	
		0.0f,
		0.0f,		
		
		0.0f, 
		stepY, 	
		
		stepX,
		stepY		
	};		

const GLfloat QuadVerticesOnly[12] = 
	{
		0.5f,
		0.5f, 
		-0.5f, 
		0.5f, 
		0.5f,
		-0.5f,
	
		//2nd tri	
		- 0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f	
	
	};
	
	
public:
	Tiles();
	~Tiles();
	bool InitTiles(Game* G);
	bool Update(Game* G); // now at the moment we don't really need to do any updating of the tiles, but lets keep a prototype here in case we decide 
						 // we want to add something like animation to our tiles, this will then be the place we will do it. For now though it's a placeholder
	bool Render(Game* G);
// these will be calculated as a result of Bob's movement	
	float ScreensXcoord;
	float ScreensYcoord;
private:	
// these are going to replace the global versions kept in the OGL instance
	GLint TileVBO;  // this is where we will keep the VBO handle for our tiles
	GLuint	TileTexture; // And the Tiles will all have 1 single texture
}
;
