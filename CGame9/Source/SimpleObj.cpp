#include <iostream> // we need this to output to console this file contains cout
#include "SimpleObj.h"

using namespace std;

// we also can draw index values
//if we sent these to the gpu
//static const GLfloat QuadVertices[] = 
//{  // Square vertices with no texture
//	1, 1,0, -1,1,0, 1,-1,0, /*now 2nd tri*/ -1,1,0, -1,-1,0,	1, -1,0,	 
//};
	


// as well as 
SimpleObj::SimpleObj() 
{
	Xspeed = 0.0f; // lets make our construc
	Yspeed = 0.0f;
	ScaleX = 16.0; // this tells us how many pixels to make our texture square, we are using 16x16 so 16 give us a one to one ratio, but we can scale up or down
	ScaleY = 16.0; // default to haveing them both have the same value

} 


SimpleObj::~SimpleObj() {} // no code yet in the destructor

/************************************************************************ 
Update has to do some work, in this case we'll print out some info on our position and
move our object in pixel space, and do a few tests to look interesting
************************************************************************/

void SimpleObj::Update()
{		// this is a tile, so it does not really need any code, its set up when it was made

		SimpleObj::Draw();   // just draw at the xy coordinates
	

}


/************************************************************************
The Draw system
************************************************************************/
void SimpleObj::Draw() 
{
// All this code is currently replicated in the Bob/PointToPoint and WaitAndChase classes
// we can decide to use just this code and remove the duplicates, or make small modifications to the draw systems for each type
// that is often preffered when you want to add different effects or shader features to a specific object type.
// for now, other than using my memory duplicating, it does no real harm to keep this default saftey copy, but try to 
// make sure you are calling the correct draw function.
	
	// by flipping the y axis here, we can now make sure our Xpos and Ypos relate to Map locations
		simpleVec2 OurScreenPosition = { Xpos, 768-Ypos };
		simpleVec2 ScreenData = { 512, 384 };  	// we only need half the screen size which is currently a fixed amount
		simpleVec2 Scale = { ScaleX, ScaleX };

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

// this is a different update, in SimpleObj, its not going to do much really
// but the classes that inheret SimpleObj will use their own versions it.

bool SimpleObj::Update(Game* TheGameClass)
{
	// as a
	return true;
}



// let the ball test to see if it hit any bullets

bool SimpleObj::DidIGetHit(SimpleObj* Obj2)
 // a default box check
{
	// simple box check using AABB principles
	// box 1 we have to remember our 16x16 square has its centre in the middle so adjust to the edges
	int x1Min = Xpos - 8;   // we know the size of these so we can use hard numbers
	int x1Max = Xpos + 8;
	int y1Min = Ypos - 8;
	int y1Max = Ypos + 8;
	
	// box 2 
	int x2Min = Obj2->Xpos - (Obj2->width / 2);   // adjust from the centre, we don't know the size of this, but we know its stored use half values to move to the top
	int x2Max = Obj2->Xpos + (Obj2->width / 2);
	int y2Min = Obj2->Ypos - (Obj2->width / 2);
	int y2Max = Obj2->Ypos + (Obj2->width / 2);
	
	// check x 1st as its more likely to fail
	if(x1Max < x2Min || x1Min > x2Max) return false;
	if (y1Max < y2Min || y1Min > y2Max) return false;
	
	return true;
}
