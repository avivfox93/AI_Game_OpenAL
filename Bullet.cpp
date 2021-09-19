//
// Created by Aviv on 29/06/2021.
//

#include "Bullet.h"
#include <cmath>
#include <iostream>
#include "Game.h"

void Bullet::_Tick(vector<Entity *> &entities) {
    nextLocation = location + Point2D(cos(angle), sin(angle));
    for (Entity *e : Game::GetInstance().GetEntities()) {
        auto *player = dynamic_cast<Player *>(e);
        if (player != nullptr) {
            if (player->GetTeam() != GetTeam() && location.Distance(player->GetLocation()) < 1.0) {
                player->Hit(power);
                alive = false;
                break; // NOLINT(readability-misleading-indentation)
            }
        }
    }
    if(dynamic_cast<Wall*>(Game::GetInstance().BOARD[(int)round(nextLocation.x)][(int)round(nextLocation.y)])){
        bulletHitSoundSource.Play();
        alive = false;
    }
}

bool Bullet::PlayingSound() {
    return bulletHitSoundSource.Playing();
}
