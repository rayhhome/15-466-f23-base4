//author: Sirui Huang, and all referenced sources
#pragma once

#include "GL.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <freetype/freetype.h>

#include <map>
#include <string>

struct Text {
  //https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
  //Holds all state information relevant to a character as loaded using FreeType
  struct Character {
      unsigned int TextureID; // ID handle of the glyph texture
      glm::ivec2   Size;      // Size of glyph
      glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
      unsigned int Advance;   // Horizontal offset to advance to next glyph
  };

  //contructor & destructor
  Text(std::string const &filename);
  ~Text();

  //glu vertex array object/vertex buffer object
  GLuint vao;
  GLuint vbo;

  //freetype info
  //reference: https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	//FreeType objects to use
	FT_Library ft_library;
  FT_Face ft_face;
  FT_Error ft_error;

  //main text display function
  void init_glyph(GLuint cur_char);
	void show_text(std::string text, glm::uvec2 const &drawable_size, float x, float y, float scale, glm::vec3 color);
};
