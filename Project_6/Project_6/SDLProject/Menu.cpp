#include "Menu.hpp"

GLuint menuFontTexture;

void Menu::Initialize(){
    state.nextScene = -1;
    menuFontTexture = Util::LoadTexture("myFont.png");
    
}
void Menu::Update(float deltaTime){}
void Menu::Render(ShaderProgram *program){
    Util::DrawText(program, menuFontTexture, "ESCAPE THE MAZE", 0.6f, -0.15f, glm::vec3(-3.0,0.5,0));
    Util::DrawText(program, menuFontTexture, "Press Enter to Start", 0.4f, -0.15f, glm::vec3(-2.75,-1.0,0));
    
}
