//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_HEALTHBOX_H
#define TEST_HEALTHBOX_H

#include "Entity.h"
#include "Player.h"

const int HEALTH_BOX_POINTS = 50;

class HealthBox : public Entity{
public:
    explicit HealthBox(int x = 0, int y = 0) : Entity(x,y){}
    void GiveHealth(Player* player){
        int healthTaken = std::min(HEALTH_BOX_POINTS,PLAYER_MAX_HEALTH - player->GetHealth());
        player->Hit(-healthTaken);
        health -= healthTaken;
        if(health <= 0) {
            alive = false;
        }
    }

protected:
    void _Tick(vector<Entity *> &entities) override {}

private:
    int health = HEALTH_BOX_POINTS;
};

#endif //TEST_HEALTHBOX_H
