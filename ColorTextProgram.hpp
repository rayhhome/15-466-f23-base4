//https://learnopengl.com/In-Practice/Text-Rendering
//also referencing the other two programs in this repo
#pragma once

#include "GL.hpp"
#include "Load.hpp"

//Shader program that draws projected, colored texts:
struct ColorTextProgram {
  ColorTextProgram();
  ~ColorTextProgram();

  GLuint program = 0;
  //Uniform (per-invocation variable) locations:
  GLuint PROJECTION_mat4 = -1U;
  GLuint TEXT_COLOR_vec3 = -1U;
  //Textures:
	//TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< ColorTextProgram > color_text_program;
