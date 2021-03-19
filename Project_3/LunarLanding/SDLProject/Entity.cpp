//
//  Entity.cpp
//  SDLProject
//
//  Created by Katrina Paredes on 3/18/21.
//  Copyright © 2021 ctg. All rights reserved.
//

#include "Entity.hpp"

Entity::Entity() {
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other) { //check if we collide with something
    if (isActive == false || other->isActive == false) {
        return false;
    }
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
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

void Entity::CheckCollisionX(Entity * objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
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

void Entity::Update(float deltaTime, Entity *platform, int platformCount) {
    if (isActive == false) { //skip. don't update
        return;
    }
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            
            if (animTime >= 0.25f){
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames) {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }
    
    //may have to change this
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionY(platform, platformCount);
    
    position.x += velocity.x * deltaTime;
    CheckCollisionX(platform,platformCount);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index){
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float) (index /animCols) / (float)animRows;
}
