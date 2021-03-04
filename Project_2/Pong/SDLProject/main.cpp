/*
 Katrina Mae Paredes
 CS-UY 3113
 Project 2 - Pong
 3 March 2021
 
 Instructions:
 Press spacebar to start the game.
 Press R to reset the game.
 Left Player: W & S
 Right Player: Up & Down
 */

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, leftPlayerMatrix, rightPlayerMatrix, ballMatrix;

//Left
glm::vec3 left_position = glm::vec3(-4.6,0,0);
glm::vec3 left_movement = glm::vec3(0,0,0);

//right
glm::vec3 right_position = glm::vec3(4.6,0,0);
glm::vec3 right_movement = glm::vec3(0,0,0);

//ball
glm::vec3 ball_position = glm::vec3(0,0,0);
glm::vec3 ball_movement = glm::vec3(0,0,0);

float player_speed = 1.5f;

GLuint leftTextureID, rightTextureID, ballTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Let's Play Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    //Object Matrices
    leftPlayerMatrix = glm::mat4(1.0f);
    rightPlayerMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    
    glUseProgram(program.programID);
    
    glClearColor(0.62f, 0.78f, 0.882f, 1.0f); //RGBa
    
    glEnable(GL_BLEND);
    
    //Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    leftTextureID = LoadTexture("CloudPixArt.png");
    rightTextureID = LoadTexture("CloudPixArt.png");
    ballTextureID = LoadTexture("SunPixelArt.png");
    
}

void ProcessInput() {
    left_movement = glm::vec3(0);
    right_movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                    case SDLK_LEFT:
                        //Move the player left
                        break;
                    case SDLK_RIGHT:
                        //move the player right
                        
                        break;
                    case SDLK_SPACE:
                        //some sort of action
                        break;
                }
                break;
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    //only using up and down movement
    if (keys[SDL_SCANCODE_W]) {
        if (left_position.y > 3.70){
            left_movement.y = 0.0f;
        }
        else {
            left_movement.y = 1.5f;
        }
    }
    else if (keys[SDL_SCANCODE_S]) {
        if (left_position.y < -3.75) {
            left_movement.y = 0.0f;
        }
        else{
            left_movement.y = -1.5f;
        }
    }
    
    if (keys[SDL_SCANCODE_UP]){
        if (right_position.y > 3.70){
            right_movement.y = 0.0f;
        }
        else {
            right_movement.y = 1.5f;
        }
    }
    else if (keys[SDL_SCANCODE_DOWN]){
        if (right_position.y < -3.75) {
            right_movement.y = 0.0f;
        }
        else{
            right_movement.y = -1.5f;
        }
    }
    
    if (keys[SDL_SCANCODE_R]){ //Press R to restart game
        left_position.y = 0;
        right_position.y = 0;
        ball_movement = glm::vec3(0,0,0);
        ball_position = glm::vec3(0,0,0);
    }
    if (keys[SDL_SCANCODE_SPACE]){
        if (ball_position.x == 0.0f){
            ball_movement.x = 1.0f;
        }
    }
    
    
    
    if (glm::length(left_movement) > 1.0f) {
        left_movement = glm::normalize(left_movement);
    }
    
    if (glm::length(right_movement) > 1.0f) {
        right_movement = glm::normalize(right_movement);
    }
}

float lastTicks = 0.0f; //ticks = amount of time passed since initialize SDL

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    //Add (direction* units per second * elapsed time)
    left_position += left_movement * player_speed * deltaTime;
    right_position += right_movement * player_speed * deltaTime;
    ball_position += ball_movement * player_speed * deltaTime;
    
    //collision detection
    //checking for collision with left player
    float lx_dist = fabs(left_position.x - ball_position.x) - ((2.0)/2.0f); //check width
    float ly_dist = fabs(left_position.y - ball_position.y) - ((2.0)/2.0f); //check height
    if (lx_dist < 0 && ly_dist < 0){ //colliding
        ball_movement.x = -ball_movement.x;
        ball_movement.y = (ball_position.y - left_position.y) - 1.5f;
    }
    
    //checking collision with right character
    float rx_dist = fabs(right_position.x - ball_position.x) - ((2.0)/2.0f); //check w&h
    float ry_dist = fabs(right_position.y - ball_position.y) - ((2.0)/2.0f);
    if (rx_dist < 0 && ry_dist < 0){ //colliding
        ball_movement.x = -ball_movement.x;
        ball_movement.y = (ball_position.y - right_position.y) - 1.5f;
       
        
    }
    
    //checking collision with sides
    if (ball_position.x > 4.9 || ball_position.x < -4.9) {
        ball_movement = glm::vec3(0,0,0);
    }
    if (ball_position.y > 3.70 || ball_position.y < -3.70){
        ball_movement.y = -ball_movement.y;
    }
    
    
    //left
    leftPlayerMatrix = glm::mat4(1.0f);
    leftPlayerMatrix = glm::translate(leftPlayerMatrix, left_position);
    leftPlayerMatrix = glm::rotate(leftPlayerMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //turn left player 90 degrees
    leftPlayerMatrix = glm::scale(leftPlayerMatrix, glm::vec3(1.5f, 1.0f, 1.0f));
    
    //right
    rightPlayerMatrix = glm::mat4(1.0f);
    rightPlayerMatrix = glm::translate(rightPlayerMatrix, right_position);
    rightPlayerMatrix = glm::rotate(rightPlayerMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //turn left player 90 degrees
    rightPlayerMatrix = glm::scale(rightPlayerMatrix, glm::vec3(1.5f, 1.0f, 1.0f));
    
    //ball
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);
    
    
}

//Draw Matrix function
void DrawMatrix(glm::mat4 aMatrix, GLuint textureID){ //may rename textureID
    program.SetModelMatrix(aMatrix);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
};


void Render() {
    
    
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    glClear(GL_COLOR_BUFFER_BIT); //clear screen
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    //Object Matrices
    DrawMatrix(leftPlayerMatrix, leftTextureID);
    DrawMatrix(rightPlayerMatrix, rightTextureID);
    DrawMatrix(ballMatrix, ballTextureID);
    
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
