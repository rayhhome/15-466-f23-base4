#include "GL.hpp"
#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Text.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct choice{
		std::string text;
		int jump;
	};

struct story{
		std::string text;
		choice First;
		choice Second;
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	std::vector<story> parseStory();

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} space, enter, reset;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//text
	Text *text = nullptr;

	std::vector <story> storyLine = {};

	std::string mainText = "";
	std::string choice1Text = "";
	std::string choice2Text = "";

	int curStory = 0;
	int curChoice = 0;

	int extraEnter = 0;

	glm::vec3 main_color     = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 choice_color_1 = glm::vec3(1.0f, 0.984f, 0.0f);
  glm::vec3 choice_color_2 = glm::vec3(0.0f, 0.0f, 0.0f);
};
