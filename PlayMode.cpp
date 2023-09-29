#include "PlayMode.hpp"

#include "ColorTextureProgram.hpp"
#include "GL.hpp"
#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Text.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"


#include <cstdint>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>
#include <sstream>
#include <string>

Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

Load< Text > text_display(LoadTagDefault, []() -> Text const * {
	return new Text(data_path("Mooli-Regular.ttf"));
});

PlayMode::PlayMode() : scene() {

	//added stuff

	storyLine = parseStory();

	curStory = 0;

	extraEnter = 0;
	mainText = storyLine[0].text;
	choice1Text = storyLine[0].First.text;
	choice2Text = storyLine[0].Second.text;
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_RETURN) {
			enter.downs += 1;
			enter.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			//reset game
			reset.downs += 1;
			reset.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_RETURN) {
			enter.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_r) {
			reset.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (space.downs > 0){
		auto tmp = choice_color_1;
    choice_color_1 = choice_color_2;
    choice_color_2 = tmp;
		if (curChoice == 0){
			curChoice = 1;
		}else{
			curChoice = 0;
		}
	} else if (enter.downs > 0){
		if (curStory == 9) {
			extraEnter += 1;
		} else {
			extraEnter = 0;
		}
		int jump = 0;
		if (curChoice == 0){
			jump = storyLine[curStory].First.jump;
		}else{
			jump = storyLine[curStory].Second.jump;
		}
		curStory = jump;
		mainText = storyLine[curStory].text;
		choice1Text = storyLine[curStory].First.text;
		choice2Text = storyLine[curStory].Second.text;
	} else if (reset.downs > 0) {
		curStory = 0;
		extraEnter = 0;
		mainText = storyLine[0].text;
		choice1Text = storyLine[0].First.text;
		choice2Text = storyLine[0].Second.text;
	}

	//reset button press counters:
	space.downs = 0;
	enter.downs = 0;
	reset.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	if (curStory == 9) {
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	} else if (curStory == 8) {
		glClearColor(0.7f, 0.3f, 0.3f, 1.0f);
	} else if (curStory >= 18 && curStory <= 22) {
		glClearColor(0.0f, 0.7f, 0.0f, 1.0f);
	} else if (curStory == 26) {
		glClearColor(0.7f, 0.2f, 0.3f, 1.0f);
	} else if (curStory == 34) {
		glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	} else if (curStory == 48) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	} else if (curStory == 50 || curStory == 51) {
		glClearColor(0.17f, 0.1f, 0.1f, 1.0f);
	} else {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	}
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	{
		glDisable(GL_DEPTH_TEST);
		std::istringstream iss(mainText);
    std::string line;
    float y = 630.0f;
    while (std::getline(iss, line, '\n')){
        text_display->show_text(line, drawable_size, -1.0f, y, extraEnter+2.0f, main_color);
				y -= 60.0f;
    }
		text_display->show_text(choice1Text, drawable_size, -1.0f, 200.0f, extraEnter+2.0f, choice_color_1);
		text_display->show_text(choice2Text, drawable_size, -1.0f, 100.0f, extraEnter+2.0f, choice_color_2);
	}
	GL_ERRORS();
}

// learned c++ file parsing from here: https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c

// learned the idea of parsing story from a txtfile to vector of struct from the work of a previous student in this course: Sarah Pethani
// https://github.com/spethani/15-466-game4
std::vector<story> PlayMode::parseStory(){

	std::vector<story> storyLineL = {};

	std::ifstream infile(data_path("story.txt"));
	
	std::string line;
	story newStory;
	std::string cur_text;
	while (std::getline(infile, line)){

		if (line.substr(0,1) == "["){
			newStory = {};
		} 
		else if (isdigit(line[0]) == false){
			cur_text+=line;
			cur_text+="\n";
		}else{
			newStory.text = cur_text;
			cur_text = "";
			
			
			newStory.First.jump = std::stoi(line.substr(0,2));
			newStory.First.text = line.substr(2);

			std::string line2;
			std::getline(infile,line2);
			newStory.Second.jump = std::stoi(line2.substr(0,2));
			newStory.Second.text = line2.substr(2);
			storyLineL.push_back(newStory);
		}
	}
	return storyLineL;
}