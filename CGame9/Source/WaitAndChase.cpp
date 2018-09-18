
#include "WaitAndChase.h"


// just like the Point to Point, we are loding the graphics in for each instance of the type we make.....is that a good idea? How can we improve that?

WaitAndChase::WaitAndChase()

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
		unsigned char* data = stbi_load(ImageNames[i], &Grwidth, &Grheight, &comp, 4);        // ask it to load 4 componants since its rgba
		//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
		// be careful to make sure you load the graphics correctly?

		Images[i] = SimpleObj::TheOGLInstance->CreateTexture2D(Grwidth, Grheight, (char*) data);        //just pass the width and height of the graphic, and where it is located and we can make a texture
		free(data);    // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before we try to load another or it causes a leak
	}

	BaseAnim = 2;   // 0 is static frame 1 is s stand, 2 and 3 are the walk frames
	Yspeed = Xspeed = 0;
	AnimIndex = 0;
	Dir = Right;
	TextureID = Images[0];

	ScaleX = 32.0;   // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;
}

// A small helper function which returns the distance this object is from the Bob player which is always in position 0 of MyObjects
float WaitAndChase::GetDistance()
{
	float Xdist = (Game::MyObjects[0]->Xpos - Xpos);
	float Ydist = (Game::MyObjects[0]->Ypos - Ypos);
	float Dist = (Xdist*Xdist) + (Ydist*Ydist);
	return sqrt(Dist);

}


//Wait and chae is simply testing to see how far Bob is, if he's close he wakes up, if he gets closer he chases him
// Also notice the WaC ojbect tests all the possible scuares it might bump into to see if there is an objstacle in the
// what that forces him to change direction. If Bob gets away he goes back to sleep.
// we're using some hard numbers such as 16 to indicate the size of a tile width, normally we would use a variable

bool WaitAndChase::Update(Game* G)
{
	float Speed = 1.2;
	Yspeed += 1.2f;

	// lets check if he's moving if not should we make him
		if(Moving == false)
	{
		BaseAnim =  AnimIndex = 0;
		Distance = GetDistance();
		if (Distance < 16 * 8) AnimIndex = 1;  // pop up and show interest
		if(Distance < 16 * 5)
		{
			Moving = true;
			Dir = (Game::MyObjects[0]->Xpos > Xpos) ? Right : Left;
		}
	}
	else
	{
		if (GetDistance() > 16 * 8) Moving = false;  // reset to wait if far away
		BaseAnim = 2;  // we are walking


		// we're moving so do the directional switches
	switch(Dir)
		{
		case Left:
			{
				Xpos -= Speed;
				// now we check for possible obsticles // we will scan from the top to the bottom of the sprite


				int YMap = (Ypos-16) / 16;
				int XMap = (Xpos - 16) / 16;

				for (int i = 0; i < 32 / 16; i++, YMap++) // 32 is the height of the character (it might however be variable)
				{
					int	WhatsAtTheEdge = G->Map2[YMap][XMap];
					int Attrib = G->Attributes[WhatsAtTheEdge];
					if (Attrib & SOLID)
					{
						Dir = Right;
						break; // break the loop we are done
					}
				}
				break;
			}
		case Right:
			{
				{
					Xpos += Speed;
					// now we check for possible obsticles
					int YMap = (Ypos-16) / 16;
					int XMap = (Xpos + 1 + 16) / 16;

					for (int i = 0; i < 32 / 16; i++)
					{
						int	WhatsAtTheEdge = G->Map2[YMap][XMap];
						;
						int Attrib = G->Attributes[WhatsAtTheEdge];

						if (Attrib & SOLID)
						{
							Dir = Left;
							break; // break the loop we are done
						}

						YMap++;  // move downp to the next
					}
					break;
				}
				break; // break the case
			}

		default:
			{
				printf("default occured, setting direction to Rightt \n");
				Dir = Right;
			}
		} // switch dir
	} // else
		// check for gravity and get the frame
	if(Yspeed > 9.81f / 4) Yspeed = 9.81f / 4;
	Ypos += Yspeed;

	int YMap = (Ypos + 17) / 16;
	int XMap = (Xpos + 8) / 16;

	// we could also choose to not let him fall!
	int	WhatsUnderOurFeet = G->Map2[YMap][XMap];
	int Attrib = G->Attributes[WhatsUnderOurFeet];


// lets make him land a bit better
	if ((Attrib & SOLID) && Yspeed >= 0)
	{
		Ypos = YMap * 16 - 16;
		Yspeed = 0;
	}

	TextureID = Images[BaseAnim + AnimIndex];
	return true;
}


// just like Bob and Point to Point this is a standard simple draw system,
// we "could" move these all into the SimpleObj class and only call one, but it allows us
// the chanse to do slightly different things to the draw "if" we want to.

void WaitAndChase::Draw()
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
