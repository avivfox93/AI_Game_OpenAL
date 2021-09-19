//
// Created by Aviv on 29/06/2021.
//

#include <iostream>
#include "Grenade.h"
#include "Game.h"
#include "cmath"

void Grenade::_Tick(vector<Entity *> &entities) {
    if(location.Distance(target) < 1.0){
        Explode();
    }else{
        nextLocation = location + Point2D(cos(angle),sin(angle));
        if(dynamic_cast<Wall*>(Game::GetInstance().BOARD[(int)round(nextLocation.x)][(int)round(nextLocation.y)])){
            Explode();
        }
    }
}

void Grenade::Explode() {
    explosionAudioSource.Play();
    for(int i = 0 ; i < GRENADE_NUM_OF_BULLETS ; i++){
        auto bullet = new Bullet((int)location.x, (int)location.y, team);
        bullet->SetAngle((i*2*M_PI)/GRENADE_NUM_OF_BULLETS);
        Game::GetInstance().GetEntities().push_back(bullet);
    }
    alive = false;
}

bool Grenade::PlayingSound() {
   return explosionAudioSource.Playing();
}
