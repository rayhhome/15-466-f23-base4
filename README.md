# Just ENTER the Game

Author: Sirui Huang, Sizhe Chen

Design: Just Enter and you'll know.

Text Drawing: 

We create Text.cpp/Text.hpp to help with the rendering process of the texts. During init, we first turn ttf into glyph textures.
Then, when we need to draw any text, it reads std::string and match the right texture to render at run time. 

Choices: 

Game choices are written and stored in txt file that follows a specific format. They are authored through our txt file editors.
At runtime, when the player starts the game, the game will parse the story and turn it into a network of story components. 


Screen Shot:

![Screen Shot](screenshot1.png)

How To Play:

Space to toggle between choices. Enter to make the choice. R to restart.

Sources: 

Assets are created on our

This game was built with [NEST](NEST.md).

