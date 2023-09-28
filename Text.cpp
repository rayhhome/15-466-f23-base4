//author: Sirui Huang, and all referenced sources
#include "Text.hpp"

#include "GL.hpp"
#include "gl_errors.hpp"
#include "gl_compile_program.hpp"

#include <hb.h>
#include <hb-ft.h>

#include "data_path.hpp"
#include "ColorTextProgram.hpp"

#include <string>
#include <iostream>
#include <cmath>

#define DEFAULT_SIZE 24
#define DRAW_WIDTH 1280
#define DRAW_HEIGHT 720

Text::Text(std::string const &filename) {
  //https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
  //FreeType initialization
  ft_error = FT_Init_FreeType(&ft_library);
  if (ft_error) {
    std::cout << "Error: FreeType Library Initialization Failure" << std::endl;
		abort();
	}
  ft_error = FT_New_Face(ft_library, filename.c_str(), 0, &ft_face);
	if (ft_error) {
		std::cout << "Error: FreeType TypeFace Initialization Failure" << std::endl;
    abort();
	}
  ft_error = FT_Set_Char_Size(ft_face, 0, DEFAULT_SIZE*64, 0, 0);
	if (ft_error) { // 72dpi
    std::cout << "Error: FreeType Set Char Size Failure" << std::endl;
		abort();
	}
  
  //https://learnopengl.com/In-Practice/Text-Rendering
  //remove restriction on byte alignment
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

  //https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
  //gl setup
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  //set up program's color and projection matrix
  glUseProgram(color_text_program->program);
	glUniform3f(glGetUniformLocation(color_text_program->program, "TEXT_COLOR"), 1.0f, 1.0f, 1.0f);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(DRAW_WIDTH), 0.0f, static_cast<float>(DRAW_HEIGHT));
	glUniformMatrix4fv(glGetUniformLocation(color_text_program->program, "PROJECTION"), 1, GL_FALSE, &projection[0][0]);
  glUseProgram(0);

  //gl objects preparation
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);   

  //generate gl errors
  GL_ERRORS();
}

Text::~Text() {
  //free freetype objects
  FT_Done_Face(ft_face);
  FT_Done_FreeType(ft_library);
}

void Text::show_text(std::string text, glm::uvec2 const &drawable_size, float x, float y, float scale, glm::vec3 color) {
	//https://learnopengl.com/In-Practice/Text-Rendering  
  //pull up shade program
  glUseProgram(color_text_program->program);
  glUniform3f(glGetUniformLocation(color_text_program->program, "TEXT_COLOR"), color.x, color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao);


  //Harfbuff buffer initialization
  hb_buffer_t *hb_buffer;
	hb_buffer = hb_buffer_create();

	//add text to buffer, using c_str to ensure text null-terminated
	hb_buffer_add_utf8(hb_buffer, text.c_str(), -1, 0, -1);
	hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
  hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));

	//create font
	hb_font_t *hb_font;
	hb_font = hb_ft_font_create(ft_face, NULL);

	//shape!!
	hb_shape(hb_font, hb_buffer, NULL, 0);

	//get all glyph information
	unsigned int len = hb_buffer_get_length(hb_buffer);
	hb_glyph_info_t *info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
	hb_glyph_position_t *gly_pos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

  // if x is less than 0, choose the position base on log2, as in a heap structure
  if (x < 0.0f) {
    int param = -static_cast<int>(floor(x));
    x = 0.0f;
    for (uint32_t i = 0; i < len; i++) {
      x += static_cast<float>((gly_pos[i].x_advance >> 6)) * scale;
    }
    // choose the center position
    int level = static_cast<int>(pow(2, 1 + floor(log2(param))));
    x = static_cast<float>((param % (level / 2)) * 2 + 1) * ((DRAW_WIDTH) / (float)(level)) - x / 2.0f;
  }
  
  if (y < 0.0f) {
    int param = -static_cast<int>(floor(y));
    y = 0.0f;
    for (uint32_t i = 0; i < len; i++) {
      y += static_cast<float>((gly_pos[i].y_advance >> 6)) * scale;
    }
    // choose the center position
    int level = static_cast<int>(pow(2, 1 + floor(log2(param))));
    y = static_cast<float>((param % (level / 2)) * 2 + 1) * ((DRAW_HEIGHT) / (float)(level)) - y / 2.0f;
  }

  //render all glyphs
	for (uint32_t i = 0; i < len; i++) {
    //get glyph codepoint
		hb_codepoint_t glyph = info[i].codepoint;

    //load and render glyphs
    ft_error = FT_Load_Glyph(ft_face, glyph, FT_LOAD_DEFAULT);
		if (ft_error) {
      std::cout << "Error: FreeType Load Glyph Failure" << std::endl;
      abort();
    }
    ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
		if (ft_error) {
      std::cout << "Error: FreeType Render Glyph Failure" << std::endl;
      abort();
    } 

    //set texture options
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      ft_face->glyph->bitmap.width,
      ft_face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      ft_face->glyph->bitmap.buffer
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //calculate size with scale
		float w = ft_face->glyph->bitmap.width * scale;
    float h = ft_face->glyph->bitmap.rows * scale;

    //calculate position with scale
		float xpos = x + (gly_pos[i].x_offset + ft_face->glyph->bitmap_left) * scale;
    float ypos = y + (gly_pos[i].y_offset + ft_face->glyph->bitmap_top) * scale - h; // note!!

    // update VBO for each character
    float vertices[6][4] = {
        {xpos  , ypos+h, 0.0f, 0.0f},            
        {xpos  , ypos  , 0.0f, 1.0f},
        {xpos+w, ypos  , 1.0f, 1.0f},
      
			  {xpos  , ypos+h, 0.0f, 0.0f},
        {xpos+w, ypos  , 1.0f, 1.0f},
        {xpos+w, ypos+h, 1.0f, 0.0f}           
    };

    //update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    //render!!
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //advance cursors for next glyph
    //bitshift by 6 to get value in pixels (2^6 = 64)
		x += (gly_pos[i].x_advance >> 6) * scale;
		y += (gly_pos[i].y_advance >> 6) * scale;
    //unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
	}

  //unbind
	glUseProgram(0);
  glBindVertexArray(0);

  //buffer use complete, free storage
	hb_buffer_destroy (hb_buffer);
  hb_font_destroy (hb_font);

  GL_ERRORS();
}