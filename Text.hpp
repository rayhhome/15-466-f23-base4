//author: Sirui (Ray) Huang, and all referenced sources (in comments as links)
//usage : Render text in quads!
#pragma once

#include "glm/gtc/type_ptr.hpp"

#include <string>

struct Text {
  //https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
  //contructor & destructor
  Text(std::string const &filename);
  ~Text();

  //main text display function
	void show_text(std::string text, glm::uvec2 const &drawable_size, float x, float y, float scale, glm::vec3 color) const;
};
