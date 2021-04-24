#include "Level3.hpp"

#define LEVEL3_WIDTH 20
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMYCOUNT 1

unsigned int level3_data[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {
    
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    //Intialize Game Objects
    state.door = new Entity();
    state.door->entityType = DOOR;
    state.door->position = glm::vec3(18,-6,0);
    state.door->width = 0.2f;
    state.door->height = 0.2f;
    state.door->textureID = Util::LoadTexture("platformPack_tile049.png");
    state.door->Update(0, state.player, NULL, NULL, state.map);
    
    //Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(2, 0, 0);
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
    
    
    state.enemies = new Entity[LEVEL3_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("pacmanGhost.png");
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].height = 0.8f;
    state.enemies[0].width = 0.8f;
    state.enemies[0].startX = 15;
    state.enemies[0].moveLeft = 1;
    state.enemies[0].moveRight = 2;
    state.enemies[0].position  = glm::vec3(15, -6, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].movement = glm::vec3(1,0,0);
    state.enemies[0].isActive = true;
}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMYCOUNT, state.map);
    
    for (int i = 0; i < LEVEL3_ENEMYCOUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMYCOUNT, state.map);
    }
    
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.door->Render(program);
    for (int i = 0; i < LEVEL3_ENEMYCOUNT; i++) {
        state.enemies [i].Render(program);
    }
    state.player->Render(program);
}
