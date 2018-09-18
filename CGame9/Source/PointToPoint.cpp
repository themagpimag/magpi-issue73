#include "PointToPoint.h"


PointToPoint::PointToPoint()

{
	char* ImageNames[] = {
		(char*)"Assets/fungus_1.png",
		(char*)"Assets/fungus_2.png",
		(char*)"Assets/fungus_3.png",
		(char*)"Assets/fungus_4.png"
	};

// lets turn these all these graphic files into textures, but consider, is it good to have them all as seperate textures?
for(int i = 0 ; i < 4 ; i++)
	{
		int Grwidth, Grheight, comp;
		unsigned char* data = stbi_load(ImageNames[i], &Grwidth, &Grheight, &comp, 4);       // ask it to load 4 componants since its rgba
		//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
		// be careful to make sure you load the graphics correctly?

		Images[i] = SimpleObj::TheOGLInstance->CreateTexture2D(Grwidth, Grheight, (char*) data);       //just pass the width and height of the graphic, and where it is located and we can make a texture
		free(data);   // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak
	}

	BaseAnim = 2;  // 0 is static frame 1 is s stand, 2 and 3 are the walk frames
	Yspeed = Xspeed = 0;
	AnimIndex = 0;
	Dir = Right;
	TextureID = Images[0];

	ScaleX = 32.0;  // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;
}



PointToPoint::~PointToPoint()
{
}
// this is a small helper function to let us set up a point to point
// it will allow is to set the startung or minx point and a distance  from that p
// where he should turn back

void PointToPoint::SetXPoints(int minx, int count, Direction d)
{
	Xmin = minx;
	Xmax = minx + count;
	Dir = d;
}

// this is the main update funciton for a point to point
// we've included the ability to do up and down, but not yet coded it
// why not find some nice graphics and try adding a flying object that moves up and down?

bool PointToPoint::Update(Game* G)
{
	float Speed = 1.2;

	Yspeed += 1.2f; // always add gravity to the yspeed, make corrections when you know if you land on something

	AnimIndex++;  // we will discover that just incrementing the index works, but it switches too fast, how can we make that more effective?

	if(AnimIndex > 1) AnimIndex = 0;

	switch(Dir)
	{
	case Left:
		{
			Xpos -= Speed;
			if (floor(Xpos) <= Xmin) Dir = Right;  // we don't check for any obstruction since he's placed were it is known there are no obstrucitons
													// but to make him more intelligent and allow changes in the map, a test for solid obstructions is a good ad.
			break;
		}
	case Right:
		{
			Xpos += Speed;
			if (floor(Xpos) >= Xmax) Dir = Left;
			break;
		}
	case Up:
		{
			break;
		}
	case Down:
		{
			break;
		}

	default:
		{
			printf("default occured, setting direction to Left \n");
			Dir = Left;
		}
	} // switch dir

// check for gravity and get the frame
	if(Yspeed > 9.81f / 4) Yspeed = 9.81f / 4;
	Ypos += Yspeed;

	int YMap = (Ypos + 16) / 16;
	int XMap = (Xpos) / 16;


	int WhatsUnderOurFeet = G->Map2[YMap][XMap];
	int Attrib = G->Attributes[WhatsUnderOurFeet];

// if we find we landed on something, null the speed and set our feet on top of the blocks
	if ((Attrib & SOLID) && Yspeed >= 0)
	{
		Ypos = YMap * 16 - 16;
		Yspeed = 0;
	}

	this->TextureID = Images[BaseAnim + AnimIndex];

	return true;
}

void PointToPoint::Draw()
{

		/**************************************************************************************
	This is the exact same code we use to Draw Bob, but we may choose to draw our baddies in a different
	way at some point so for now using the code this way opens up options to us later.
		***************************************************************************************/

	// by flipping the y axis here, we can now make sure our Xpos and Ypos relate to Map locations
	simpleVec2 OurScreenPosition = { ScreenX * SCALEFACTOR, 1080 - (ScreenY*SCALEFACTOR) };
		simpleVec2 ScreenData = { 1920/2, 1080/2 };  	// we only need half the screen size which is currently a fixed amount
		simpleVec2 Scale = { ScaleX * SCALEFACTOR, ScaleY * SCALEFACTOR };

		glUseProgram(this->TheOGLInstance->programObject);


// this code is all explained in the SimpleOj version
			glUniform2fv(positionUniformLoc, 1, &OurScreenPosition.xValue);
			glUniform2fv(ScreenCoordLoc, 1, &ScreenData.xValue);
			glUniform2fv(ScaleLoc, 1, &Scale.xValue);
			glUniform1i(samplerLoc, 0);

			glBindBuffer(GL_ARRAY_BUFFER, SimpleObj::VBO);     //now we mind that, which we can leave as bound since we use the same VBO
			glBindTexture(GL_TEXTURE_2D, TextureID);     // we kept the texture in its own class this time

	GLuint stride = 5*sizeof(float);   // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
		3,		// there are 3 values xyz
		GL_FLOAT, // they are float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		(const void*)0  // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);

	glVertexAttribPointer(texCoordLoc,
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they are float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

	// but we will still ask it to use the same position and texture attributes locations
			glEnableVertexAttribArray(positionLoc);       // when we enable it, the shader can now use it and it starts at its base value
			glEnableVertexAttribArray(texCoordLoc);       // when we enable it, the shader can now use it
			// now its been set up, tell it to draw 6 vertices which make up a square
			glDrawArrays(GL_TRIANGLES, 0, 6);

	if (glGetError() != GL_NO_ERROR) printf(" draw errors\n");
	// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);


}
