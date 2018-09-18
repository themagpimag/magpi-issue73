#include "Tiles.h"
// We created a small Tiles class to manage the tiles, and remvoved all that extra code from the Game Class, we should try to keep the Game class tidy
// so making a new class here lets us focus all the tile functions here.



Tiles::Tiles()
{
}

Tiles::~Tiles()
{
}


// take notice, that this init code is basically the same code we had in the game class to initialise our tiles
// there are a few minor changes to allow us to access things via the Game class that calls it, and we've moved the
// arrays which held constant data into the class definition to make it a little tidier. Now our game class only needs create an instance
// of tiles and the InitTiles(this) to load and set up
// We could also move this code up to the constructor, but generally speaking if the constructor is doing a lot of things we usually have an init funciton instead
// just to keep things a bit cleaner. This is doing 3 things.. Loading&creating texure, calculating the UV coords of the tiles, and finally creating a VBO
// thats quite a lot of work, so best done in an init

bool Tiles::InitTiles(Game* G)
{


		int Grwidth, Grheight, comp;
		char* OneTileTexture =  	(char*)"images/tileset.png";  // 1st load our texture containing all our tiles
	// now lets build our buffer
	// make our texture	but just load it into the 0 entry of the array
			unsigned char* data = stbi_load(OneTileTexture, &Grwidth, &Grheight, &comp, 4);      // ask it to load 4 componants since its rgba
			TileTexture = G->OGLES.CreateTexture2D(Grwidth, Grheight, (char*) data);        //just pass the width and height of the graphic, and where it is located and we can make a texture
			free(data);   // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak

	// ok we loaded our texture, now lets create a GPU buffer called a VBO and stoe all the vertices we need to draw the whole map in there.


	// now we will build up our tiles in to our array but this time our array is made up of TileData, we could use a fixed size array, since we know how many tiles there are in our map
	// but lets continue to practice our use of vectors.
		std::vector<TileVectorData> NewTiles;

	// because we are using a single texture where each tile is a fraction of the whole we need to work out the new UV coordinates for each tile
	// each vertix needs a UV coordinate, so thats means each tile has 6 pairs of UV coordinates or a simpleVec2, so 16 tiles, s * 6 vertices

		simpleVec2 UVcoords[16 * 6];

	// now the uv which will depend on the tile we want to draw, this is a bit more complex as the old UV coordinates depended on 1 set of data for each time, here the data is going to be different
	// our tilemap has 2x8 tiles in each texture, so the step to the next textire is goig to be 1/8 for X and 1/2 for Y


	// now go through a loop of 2 rows, 8 colums, and 6 vertices (though they are pairs so a counter of 12)
		int StoreIndex = 0;
	for (int Y = 0; Y < 2; Y++)
	{
		for (int X = 0; X < 8; X++)
		{
			for (int k = 0; k < 12; k += 2)  // increment in 2's
				{
					float NewU =  QuadTexVals[k] + (stepX*X);
					float NewV =  QuadTexVals[k + 1] + (stepY*Y);

					UVcoords[StoreIndex].xValue = NewU;  // store then increment the index
					UVcoords[StoreIndex].yValue = NewV;
					StoreIndex++;
				} // end of k loop
		} // end of X loop

	} // end of Y loop

// now we have our UV coordinates stored ready to be used  any time that we want them

// lets make our playfiled the same way as last lesson but this time use a vector to store all the vertex offsets, and UV's.
// Start by scanning through the map, finding out which tile is to be drawn.
// Then work out the position of its 6 vectors and is UV coordinates
// store them all into a nice big array or vector (vector are better for this as they are more dynamic)
// Once we have a buffer transfer it to the GPU memory
// then when its time to write, use the GPU memory buffer
// All this setting up, takes a little bit of time, but its only ever done once...at a point where we're not watchng the screen
// which means the user will never see this slow setup, but will appreciate the faster update.



	for (int Row = 0; Row < 40; Row++) // this is an outer loop we will use the Row variable to go down the grid
		{
			for (int Column = 0; Column < 64; Column++) // this is an inner loop and will go along the grid
				{
					GLint WhatTile = G->Map2[Row][Column];   // what tile is it?
						// each tile has 6 vertices**, so we have to provide the data for the quad and the screen position for the shader to place it
						for(int i = 0 ; i < 6 ; i++)	 // we
					{
						TileVectorData NewTile;   // create a new instance of TileVectorData...Note, do not use new.

// now work out the  position and get the uv for this vertex
						NewTile.PositionX = QuadVerticesOnly[i * 2];  // times 2 because we are using pairs of floats
						NewTile.PositionY = QuadVerticesOnly[(i * 2) + 1];
						NewTile.dummyZ = 0;    // This is "probably" already 0 but its always best to make sure, as there's no certainty that allocated memory is blank
// send the screen position for the centre of the quad, we are repeating ourseves by sending this 6 times but it allows faster access on the GPU
						NewTile.sPositionX = ((Column * (16*SCALEFACTOR)) + 8*SCALEFACTOR);         // the tiles are 16 pixels apart in X and Y lets move them +8 because the reference is in the centre
						NewTile.sPositionY = (1080 - ((Row * (16*SCALEFACTOR)) + 8*SCALEFACTOR));


						// next we need the UV coordinate we precalulated for this tile
												NewTile.u = UVcoords[(WhatTile * 6) + i].xValue;  // we don't need to double i, since we are using simpleVec2 which already pairs them
												NewTile.v = UVcoords[(WhatTile * 6) + i].yValue;

						NewTiles.push_back(NewTile);     // place it in the vector
					} // end of for 1-6 vertice




				} // end of for column
		} // end of for row


		// now at the end of all this we have a large vector called newTiles which contains ALL the vertex and UV positions for all the tiles, in one big chunk of memory
		// we can choose to use this over and over again, which will be fast, but faster still would be to send this to a GPU buffer called a VBO and use that when we
		// draw tiles, making the whole draw process simply a case of setting up a few uniforms and then doing a single draw call.

		// setting up a VBO is 3 steps
		// 1 ask the GPU to make one for us
			GLuint VBO;
	glGenBuffers(1, &VBO);
	// 2 bind it
		glBindBuffer(GL_ARRAY_BUFFER, TileVBO);
	// 3 transfer the data to it
		glBufferData(GL_ARRAY_BUFFER, NewTiles.size() * 7 * sizeof(float), &NewTiles.at(0), GL_STATIC_DRAW);

	// all done, it seems like a lot of work, but it will be good
	// we will see the benefit of this when we do 1 draw call to draw all our objects...But...there is an even FASTER way to draw these tiles.. using indexing, but thats something you can research

	// Something else to consider, when we created the arrays and vectors, we did them all inside this function, so all the variables were created on the stack since we didn't
	// create them using new.
	// so when we finish, they will all be cleared and we don't have to clear up

		return true;
}


// if we were to animate or change some tiles as part of game play it would be done here in this update
bool Tiles::Update(Game* G)
{
	return true;
}


bool Tiles::Render(Game* G)
{

	glUseProgram(G->OGLES.FastProgramObject);  // it was created in the OGL init, so lets keep using it
	// now there's a good argument to be made for not reloading these values every time, since they never change...but for clarity we will set them up here each time

	GLuint samplerLoc, ScaleLoc, ScreenDataLoc;	  // we can predefine values we are going to use

// or define them as we use them...the choice is ours
	GLuint positionLoc = glGetAttribLocation(G->OGLES.FastProgramObject, "a_position");
	GLuint ScreenPositionLoc = glGetAttribLocation(G->OGLES.FastProgramObject, "u_position");  // remember strictly speaking we wouldn't call this a u_ for uniform but for clarity we will
	GLuint texCoordLoc = glGetAttribLocation(G->OGLES.FastProgramObject, "a_texCoord");

	// let the shaders know what VBO and texture to use
	glBindBuffer(GL_ARRAY_BUFFER, TileVBO);
	glBindTexture(GL_TEXTURE_2D, TileTexture);   // we kept the texture in its own class this time

	// set up uniforms as normal, though again these are always the same so this could also be done on an init
	simpleVec2 ScreenData = { 1920/2, 1080/2 };   	// we only need half the screen size which is currently a fixed amount
	simpleVec2 Scale = { 16*SCALEFACTOR, 16*SCALEFACTOR }; //<< hard numbers?


	samplerLoc = glGetUniformLocation(G->OGLES.FastProgramObject, "s_texture");
	ScreenDataLoc = glGetUniformLocation(G->OGLES.FastProgramObject, "u_Screensize");
	ScaleLoc = glGetUniformLocation(G->OGLES.FastProgramObject, "u_Scale");
	GLuint OffsetLoc = 	glGetUniformLocation(G->OGLES.FastProgramObject, "u_Offsets");
//each pixel is 2/width of the screen displayed
	simpleVec2 TheXYOffsets = { (2.0f/1920.0f)*16.0f, 0 };

// now turn the screen's start x y coords in to a shader usable value of offsets
	TheXYOffsets.xValue = (ScreensXcoord *(2.0f / 1920.0f));
	TheXYOffsets.yValue = (ScreensYcoord *(2.0f / 1080.0f));



	// now transfer our screen sizes and scale
	glUniform2fv(ScreenDataLoc, 1, &ScreenData.xValue);      // pass our half screen sizes by pointing to address of the 1st value in the struct
	glUniform2fv(ScaleLoc, 1, &Scale.xValue);
	glUniform2fv(OffsetLoc, 1, &TheXYOffsets.xValue);

	glUniform1i(samplerLoc, 0);


	GLuint stride = 7*sizeof(float);   // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
		3,		// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		(const void*)0  // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);

	glVertexAttribPointer(ScreenPositionLoc,
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they a float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

	// Load the texture coordinate
	glVertexAttribPointer(texCoordLoc,
		2, // there are 2 values u and c
		GL_FLOAT,	 //they are floats
		GL_FALSE,	 // we don't need them normalised
		stride, 	  // whats the stride to the next ones
		(const void*)(sizeof(float) * 5)
		);

	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(texCoordLoc);
	glEnableVertexAttribArray(ScreenPositionLoc);

// we "could" work out just the numbers of tiles we want to print and draw only a section, but
// in fact the calucation is simple but it increases the number of draw calls, which is never a good idea
// our map really isn't massive, so letting it draw it all, even if not shown is acceptable
	glDrawArrays(GL_TRIANGLES, 0, 40 * 64 * 6);

// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);
	glDisableVertexAttribArray(ScreenPositionLoc);

	return true;
}
