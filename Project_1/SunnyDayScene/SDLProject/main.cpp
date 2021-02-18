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
glm::mat4 viewMatrix, sunMatrix, projectionMatrix, cloudMatrix_1, cloudMatrix_2;

float sunPixelSize = 1; //keep track of sun position
float sunRotate = 0;
bool sunShrink = false;
float cloudPixel1 = -5;
float cloudPixel2 = 2;

GLuint sunTextureID;
GLuint cloudPixel1TextureID;
GLuint cloudPixel2TextureID;

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
    displayWindow = SDL_CreateWindow("Sunny Day!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    sunMatrix = glm::mat4(1.0f);
    cloudMatrix_1 = glm::mat4(1.0f);
    cloudMatrix_2 = glm:: mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(0.592, 0.573f, 0.89f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.521f, 0.78f, 0.949f, 1.0f); //RGBa
    
    glEnable(GL_BLEND);
    
    //Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sunTextureID = LoadTexture("SunPixelArt.png");
    cloudPixel1TextureID = LoadTexture("CloudPixArt.png");
    cloudPixel2TextureID = LoadTexture("CloudPixArt.png");
    

    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f; //ticks = amount of time passed since initialize SDL

void Update() {
    //Calculate the time since last frame
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    
    sunRotate += 12.0f * deltaTime; //rotate 90d/s
    
    //cloudPixel1 += 1.0f * deltaTime; //move 1/s
    //CLoud Motion
    if (cloudPixel1 > 6) {
        cloudPixel1 = -6;
    }
    else{
        cloudPixel1 += 0.9f * deltaTime; //move 1/s
    }
    if (cloudPixel2 > 6) {
        cloudPixel2 = -6;
    }
    else{
        cloudPixel2 += 0.6f * deltaTime; //move 1/s
    }
    
    //Sun Scaling
    if (!sunShrink){
        sunPixelSize += 0.12f * deltaTime;
    }
    else{
        sunPixelSize -= 0.12f * deltaTime;
    }
    
    if (sunPixelSize > 2 || sunPixelSize < 1) {
        sunShrink = !sunShrink;
    }

    
    sunMatrix = glm::mat4(1.0f);
    cloudMatrix_1 = glm::mat4(1.0f);
    cloudMatrix_2 = glm::mat4(1.0f);
    
    
    sunMatrix = glm::rotate(sunMatrix, glm::radians(sunRotate), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate on the Z axis
    sunMatrix = glm::scale(sunMatrix, glm::vec3(sunPixelSize, sunPixelSize, 1.0f));
    cloudMatrix_1 = glm::translate(cloudMatrix_1, glm::vec3(cloudPixel1, 2.0f, 0.0f)); //translate by what the position of cloud1 is
    cloudMatrix_1 = glm::scale(cloudMatrix_1, glm::vec3(1.5f, 1.0f, 1.0f));
    cloudMatrix_2 = glm::translate(cloudMatrix_2, glm::vec3(cloudPixel2, -1.0f, 0.0f)); //translate by what the position of cloud2 is
    cloudMatrix_2 = glm::scale(cloudMatrix_2, glm::vec3(1.2f, 1.0f, 1.0f));
    
}
//Draw Matrix function
void DrawMatrix(glm::mat4 aMatrix, GLuint textureID){
    program.SetModelMatrix(aMatrix);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
};


void Render() {
    //glClear(GL_COLOR_BUFFER_BIT);
    
    //program.SetModelMatrix(sunMatrix);
    
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    glClear(GL_COLOR_BUFFER_BIT); //clear screen
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    //Draw object to screen
    DrawMatrix(sunMatrix, sunTextureID);
    DrawMatrix(cloudMatrix_1, cloudPixel1TextureID);
    DrawMatrix(cloudMatrix_2, cloudPixel2TextureID);
    
    //program.SetModelMatrix(sunMatrix);
    //glBindTexture(GL_TEXTURE_2D, sunTextureID);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    
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
