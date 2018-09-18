#include "SimpleObj.h"
#include "Game.h"
class PointToPoint: public SimpleObj
{
public:
	PointToPoint();
	~PointToPoint();
	
	
	bool Update(Game* G);
	
	int	BaseAnim;
	int	AnimIndex;
	void Draw();
	void SetXPoints(int minx, int count, Direction d); // A small helper function

	int Xmin;
	int Xmax;
private:	
	// no other classes need to have access to these;	

	int CurrentState;
	int TestYPos;   //
	GLuint Images[12];  // handles for the images 
	
};