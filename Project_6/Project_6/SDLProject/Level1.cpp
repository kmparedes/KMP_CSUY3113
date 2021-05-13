#include "Level1.hpp"

#define LEVEL1_WIDTH 11
#define LEVEL1_HEIGHT 20

#define LEVEL1_ENEMYCOUNT 3

unsigned int level1_data[] =
{
    23, 23, 23, 23, 23, 23, 23, 23, 0, 0, 23,
    23, 23, 0, 0, 0, 0, 0, 0, 0, 0, 23,
    23, 0, 0, 0, 23, 23, 23, 23, 23, 23, 23,
    23, 0, 0, 0, 23, 23, 23, 23, 23, 23, 23,
    23, 0,  0,  0,  0,  0,  0,  0,  0, 0, 23,
    23, 23, 23, 23, 23, 23, 23, 0, 0, 0, 23,
    23, 23, 23, 23, 23, 23, 23, 0, 0, 0, 23,
    23, 0, 0, 0, 23, 23, 23, 23, 23, 0, 23,
    23, 0, 0, 0, 23, 23, 23, 23, 23, 0, 23,
    23, 23, 0,  23, 23, 23, 23, 23, 23, 0,  23,
    23, 23, 0,  23, 23, 23, 0,  0,  0,  0,  23,
    23, 0,  0,  0,  23, 23, 0,  0,  0,  23, 23,
    23, 0,  0,  0,  23, 23, 0,  0,  0,  23, 23,
    23, 0,  0,  0,  23, 23, 0,  0,  0,  23, 23,
    23, 0,  0,  0,  23, 23, 23, 0,  23, 23, 23,
    23, 23, 0,  0,  0,  0,  0,  0,  23, 23, 23,
    23, 23, 23, 23, 23, 0,  23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 0,  23, 23, 23, 23, 23,
    23, 23, 23, 23, 0,  0,  0,  23, 23, 23, 23,
    23, 23, 23, 23, 0,  0,  0,  23, 23, 23, 23
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("mapPack_tilesheet_2X.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 17, 12);
    
    //Intialize Game Objects
    
    //Initialize Door
    state.castle = new Entity();
    state.castle->entityType = CASTLE;
    state.castle->position = glm::vec3(9,0,0);
    state.castle->width = 0.2f;
    state.castle->height = 0.2f;
    state.castle->textureID = Util::LoadTexture("mapTile_100.png");
    state.castle->Update(0, state.player, NULL, NULL, state.map);
    
    //Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, -19, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("betty.png");
    
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
    
    
    
    state.enemies = new Entity[LEVEL1_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("pacmanGhost.png");
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].height = 0.7f;
        state.enemies[i].width = 0.7f;
        state.enemies[i].speed = 1;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = WALKING;
        state.enemies[i].movement = glm::vec3(1,0,0);
        state.enemies[i].isActive = true;
    }
    
    state.enemies[0].position = glm::vec3(4, -4.0f, 0);
    state.enemies[0].startX = 4;
    state.enemies[0].moveLeft = 3;
    state.enemies[0].moveRight = 5;
    
    state.enemies[1].position = glm::vec3(7.0f, -10.0f,0);
    state.enemies[1].startX = 7;
    state.enemies[1].moveLeft = 1;
    state.enemies[1].moveRight = 2;
    
    state.enemies[2].position = glm::vec3(2.0f, -12.0f, 0);
    state.enemies[2].startX = 2;
    state.enemies[2].moveLeft = 1;
    state.enemies[2].moveLeft = 1;
    
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    state.player->loseGame(state.enemies, LEVEL1_ENEMYCOUNT);
    //code for going to new level
    
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    state.castle->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    state.player->Render(program);
}
