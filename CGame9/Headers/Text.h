//  A simple wrapper class to control our text handling we only need an instance of it 
#pragma once
#include <stdio.h>
#include <iostream>
#include "string.h"
#include <map>
#include "OGL.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

// Include GLM
//#undef countof
//#include "glm/glm/glm.hpp"
//#include "glm/glm/gtc/matrix_transform.hpp"


class Text
{
public:
	Text();   //standard constructor
	~Text();   //standard destructor
	void Display(std::string text);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale);
	void SetSize(int height);
	
// define what a character is
	struct Character {
		GLuint     TextureID;   // ID handle of the  texture
		simpleVec2 Size;        // X and Y Size of glyph
		simpleVec2 Bearing;     // an offset from base to left/top of glyph
		GLuint     Advance;     // and the amount to advance to next glyph
	};
	

// a map is a way to...map, character ascii values to our character data structs
	std::map<GLchar, Character> Characters;
	GLuint	ColourUniform;

	GLint	positionloc;
	GLint	textureloc;
	FT_Library ft;
	FT_Face face;
	OGL* WhereisOGL;
}
;