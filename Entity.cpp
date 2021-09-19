//
// Created by Aviv on 29/06/2021.
//

#include "Entity.h"
#include "Game.h"

void Entity::UpdateLocation() {
    location = nextLocation;
}

void Entity::Tick(vector<Entity*>& entities) {
    if(Game::GetInstance().Ticks() > nextTick) {
        _Tick(entities);
        nextTick = Game::GetInstance().Ticks() + GAME_MAX_SPEED - speed;
    }
}
