

#include "Text.h"
#include "SimpleObj.h"




// we can set up different textures if we want but for now we'll just use 1 and hard code it
Text::Text()
{


	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR Could not init the FreeType Library" << std::endl;// if this happens...ermm you're in trouble, becuase it should be on the Raspberry by default
	
	if (FT_New_Face(ft, "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 0, &face))
		std::cout << "ERROR This font failed to load." << std::endl; 
	
	FT_Set_Pixel_Sizes(face, 0, 32);  //we need to set up a default size.. 
// now we have to set a default glyph	


  // Disable byte-alignment restriction since we really want to look at individual bytes
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
  
	// now cycle through the ascii chars from 0 - 128 and make glyphs (what Freetype calls the letters)
	
	for(int ascii= 0 ; ascii < 128 ; ascii++)
	{
		// Load character glyph 
		if(FT_Load_Char(face, ascii, FT_LOAD_RENDER))
		{
			std::cout << "ERROR We failed to load a Glyph" << std::endl;
			continue; // it may be a single issue so let the loop continue rather than break, the output will warn us
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
		    GL_TEXTURE_2D,
			0,
			GL_ALPHA,
			// just the alpha componant please
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_ALPHA,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		
		if (glGetError() != GL_NO_ERROR) 
			printf(" Font texture generation error\n");
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
// this is a little contrived, because our simpleVec2 is a very basic structure, it has to be set up before we can use it
// it would be nicer if we could set it up "as" we use it, but that comes later.
		simpleVec2 vec1 = { (GLfloat)face->glyph->bitmap.width, (GLfloat)face->glyph->bitmap.rows };
		simpleVec2 vec2 = { (GLfloat)face->glyph->bitmap_left, (GLfloat)face->glyph->bitmap_top };
		
		
// create a new instance of a character struct			
		Character character = {
			texture, 
			vec1,
			vec2,
			(GLuint)face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(ascii, character));
	}

	
// now we've set up the textures and the character maps, we don't need the libs resources or the face(texture data) any more but we don't
// delete the lib, just tell it we're done.
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Text::~Text()
{

}

void Text::SetSize(int height)
{
	FT_Set_Pixel_Sizes(face, 0, height);  	// if we use the height to determine the size, 0 for width makes it allocate dynamically
}
void Text::Display(std::string text)
{
	
	
}


void Text::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale)
{


	this->WhereisOGL->setOrthoMatrix(0, 1080, 0, 1920, 0,  100);

	
	glUseProgram(this->WhereisOGL->TextProgramObject);
	
	positionloc = glGetAttribLocation(this->WhereisOGL->TextProgramObject, "a_position");
	textureloc = glGetAttribLocation(this->WhereisOGL->TextProgramObject, "a_texCoord");
	
	
	GLint MatrixUni = glGetUniformLocation(this->WhereisOGL->TextProgramObject, "projection");
	GLint samplerLoc = glGetUniformLocation(this->WhereisOGL->TextProgramObject, "s_texture2");
	
	glUniform1i(samplerLoc, 0);
	
	glUniformMatrix4fv(MatrixUni, 1, GL_FALSE, &this->WhereisOGL->OrthoMatrix[0][0]);

// we are only going to use 1 active texture in the shader	
	glActiveTexture(GL_TEXTURE0);
	
	
	
	// Iterate through all characters
		std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.xValue * scale;
		GLfloat ypos = y - (ch.Size.yValue - ch.Bearing.yValue) * scale;

		GLfloat w = ch.Size.xValue * scale;
		GLfloat h = ch.Size.yValue * scale;
		
		
				GLfloat vertices[6][4] = {
					{ xpos,		ypos + h,	0.0, 0.0 },            
					{ xpos,		ypos,		0.0, 1.0 },
					{ xpos + w, ypos,		1.0, 1.0 },
		
					{ xpos,		ypos + h,	0.0, 0.0 },
					{ xpos + w, ypos,		1.0, 1.0 },
					{ xpos + w, ypos + h,	1.0, 0.0 }           
				};
		
	
		
		// Render glyph texture over quad

	
		glBindBuffer(GL_ARRAY_BUFFER, 0); // use the ram

		// now tell the attributes where to find the vertices, positions and uv data
	int stride = 4*sizeof(float); 
		
		glVertexAttribPointer(positionloc, 
		2,		// there are 2 values xy
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		&vertices[0][0]   // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);
		
		glVertexAttribPointer(textureloc, 
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they a float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		&vertices[0][2]     // where do they start as an index
		); 
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	// but we will still ask it to use the same position and texture attributes locations
		glEnableVertexAttribArray(positionloc);        // when we enable it, the shader can now use it and it starts at its base value
		glEnableVertexAttribArray(textureloc);         // when we enable it, the shader can now use it
		
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale;   // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
		
		glBindTexture(GL_TEXTURE_2D, 0);
	

	// its wise to disable when done
}