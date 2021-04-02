#include "Entity.hpp"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){ //check if we collide with something
    
    if (other == this) {return false;}
    
    if (isActive == false || other->isActive == false){
        return false;
    }
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) /2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) /2.0f);
    
    if (xdist < 0 && ydist < 0){
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        
        if (CheckCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height/2.0f) - (object->height/2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        
        if (CheckCollision(object)){
            
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width/2.0f) - (object->width/2.0f));
            
            
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::DoorTransport(Entity *door){ //Transport to next level
    if (CheckCollision(door) == true) {
        position.y -= 2.0f;
    }
}
 //Gameplay
void Entity::loseGame(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x) - ((width + object->width) /2.0f);
            if (xdist < 0 && (position.y < object->position.y)){
                isActive = false;
            }
        }
    }
}

void Entity::JumpAttack(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            if (position.y > object->position.y) {
                object->isActive = false;
            }
        }
    }
}

void Entity::AIWalker(){

    if (position.x < -5) {
        movement = glm::vec3(1,0,0);
    }
    else if (position.x > 5){
        movement = glm::vec3(-1,0,0);
    }
    
}

void Entity::AIAttack(Entity *player){
    if (position.y < -3){
        velocity.y += jumpPower;
    }
}


void Entity::AIWaitAndGo(Entity *player){
    switch(aiState){
        case IDLE:
            if (glm::distance(position, player->position) < 2.0f){
                if (aiType == WAITANDGO){
                    aiState = WALKING;
                }
                if (aiType == ATTACKER){
                    aiState = ATTACKING;
                }
            }
            break;
        case WALKING:
            if (player->position.x < position.x){
                movement = glm::vec3(-1,0,0);
            }
            else {
                movement = glm::vec3(1,0,0);
            }
            
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity *player){
    switch(aiType){
        case WALKER:
            AIWalker();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case ATTACKER:
            AIAttack(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *platform, int platformCount)
{
    if (isActive == false){
        return;
    }
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY) {
        AI(player); //checking process input
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            
            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump){
        velocity.y += jumpPower; //adding instantly jump power
        jump = false;
    }
    
    velocity.x = movement.x * speed; //instant velocity. Let them move
    velocity += acceleration * deltaTime; //if we're moving, keep adding to velocity
    
    position.y += velocity.y * deltaTime;       //Move on Y
    CheckCollisionsY(platform, platformCount);  //Fix if needed
    
    position.x += velocity.x * deltaTime;       //Move on X
    CheckCollisionsX(platform, platformCount);  //Fix if needed
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isActive == false){
        return;
    }
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
