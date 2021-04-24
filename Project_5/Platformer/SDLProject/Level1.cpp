#include "Level1.hpp"

#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMYCOUNT 1

GLuint fontTextureID;

unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    
    //Intialize Game Objects
    fontTextureID = Util::LoadTexture("font1.png");
    
    //Initialize Door
    state.door = new Entity();
    state.door->entityType = DOOR;
    state.door->position = glm::vec3(8,-1,0);
    state.door->width = 0.2f;
    state.door->height = 0.2f;
    state.door->textureID = Util::LoadTexture("platformPack_tile049.png");
    state.door->Update(0, state.player, NULL, NULL, state.map);
    
    //Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0); //never change. player always in freefall
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f; //fix width & height so character is not floating
    state.player->width = 0.8f;
    
    state.player->jumpPower = 5.0f;
    state.player->lives = 3;
    
    
    state.enemies = new Entity[LEVEL1_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("pacmanGhost.png");
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].height = 0.8f;
    state.enemies[0].width = 0.8f;
    state.enemies[0].startX = 12;
    state.enemies[0].moveLeft = 4;
    state.enemies[0].moveRight = 5;
    state.enemies[0].position  = glm::vec3(12, -5, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].movement = glm::vec3(1,0,0);
    state.enemies[0].isActive = true;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    
    if (state.player->CheckCollision(state.door)) {
        state.nextScene = 2;
    }
    
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.door->Render(program);
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies [i].Render(program);
    }
    state.player->Render(program);
    Util::DrawText(program, fontTextureID, "Text ", 1.0f, -0.5f, glm::vec3(1, -1, 0));

}
