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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PLATFORM_COUNT 1
#define GOAL_COUNT 2

#include "Entity.hpp"

struct GameState {
    Entity *ship;
    Entity *platform;
    Entity *goal;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


GLuint fontTextureID; //for text

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
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

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        //set up the vertices and texCoords
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height
        });
    }
    //after the vertices and texCoords are setup, we can draw using familiar code.
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //background color
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Initialize Player
    state.ship = new Entity();
    state.ship->position = glm::vec3(0, 3.75, 0);
    state.ship->movement = glm::vec3(0);
    state.ship->acceleration = glm::vec3(0, -0.1f, 0);; //never change. player always in freefall
    state.ship->speed = 1.0f;
    state.ship->textureID = LoadTexture("playerShip1_blue.png");
    
    state.ship->height = 1.0f;
    state.ship->width = 1.0f;
    
    //Intialize Platform
    state.platform = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");
    state.platform[0].textureID = platformTextureID;
    state.platform[0].position = glm::vec3(0,-3.25,0);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) { //update platforms one time
        state.platform[i].Update(0, NULL, 0, 0);
    }
    
    //initializ goal
    state.goal = new Entity[GOAL_COUNT];
    
    GLuint goalTextureID = LoadTexture("platformPack_tile007.png");
    state.goal[0].textureID = goalTextureID;
    state.goal[0].position = glm::vec3(-1,-3.25,0);
    
    state.goal[1].textureID = goalTextureID;
    state.goal[1].position = glm::vec3(-2,-3.25,0);
    
    for (int i = 0; i < GOAL_COUNT; i++) { //update platforms one time
        state.goal[i].Update(0, NULL, 0, 0);
    }
    
    //load font texture
    fontTextureID = LoadTexture("font1.png");
}


void ProcessInput() {
    state.ship->movement = glm::vec3(0);
    
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    //holding down keys
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        state.ship->movement.x = -0.79f;
        state.ship->acceleration.x -= 0.45f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.ship->movement.x = 0.79f;
        state.ship->acceleration.x += 0.45f;
    }
    
    if (glm::length(state.ship->movement) > 1.0f) {
        state.ship->movement = glm::normalize(state.ship->movement);
    }
}



#define FIXED_TIMESTEP 0.0166666f //60 frames a second
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float) SDL_GetTicks() /1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if(deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        //Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.ship->Update(FIXED_TIMESTEP, state.platform, PLATFORM_COUNT, 1);
        state.ship->Update(FIXED_TIMESTEP, state.goal, GOAL_COUNT, 2);
        
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platform[i].Render(&program);
    }
    
    for (int i = 0; i < GOAL_COUNT; i++) {
        state.goal[i].Render(&program);
    }
    
    if (state.ship->acceleration == glm::vec3(0) && state.ship->missionState == 2) {
        DrawText(&program, fontTextureID, "MISSION SUCCESSFUL", 0.5, -0.15f, glm::vec3(-4.25f, 3, 0));
    }
    
    if (state.ship->acceleration == glm::vec3(0) && state.ship->missionState == 1) {
        DrawText(&program, fontTextureID, "MISSION FAIL", 0.5, -0.15f, glm::vec3(-4.25f, 3, 0));
    }
    
    state.ship->Render(&program);
    
    
    //DrawText(&program, fontTextureID, "MISSION SUCCESSFUL", 0.5, -0.15f, glm::vec3(-4.25f, 3, 0));
    //DrawText(&program, fontTextureID, "Lives: " + std::to_string(3), 0.5f, -0.25f, glm::vec3(-3.75f, 3.3, 0));
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char*argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
