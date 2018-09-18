#pragma once
#include "SimpleObj.h"
#include "Game.h"


class WaitAndChase : public SimpleObj
{
public:
	WaitAndChase();
	~WaitAndChase();
	
	bool Update(Game* G);
	float GetDistance();
	void Draw();
	
	int	BaseAnim;
	int	AnimIndex;
	
	Direction OurHeading;
	float	Distance; 
	bool	Moving;
	
//private:	
	// no other classes need to have access to these;	

	int CurrentState;
	int TestYPos;    //
	GLuint Images[12];   // handles for the images  >>> hmmm?? We've seen this before, does it raise any questions?
	
};