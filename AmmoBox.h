//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_AMMOBOX_H
#define TEST_AMMOBOX_H

#include "Player.h"

const int AMMO_BOX_BULLETS  = 15;
const int AMMO_BOX_GRENADES = 15;

class AmmoBox : public Entity{
public:
    explicit AmmoBox(int x = 0, int y = 0) : Entity(x,y){}
    void TakeAmmo(Player* player){
        int grenadesTaken = std::min(PLAYER_MAX_GRENADES - player->GetNumOfGrenades(), grenades);
        int bulletsTaken = std::min(PLAYER_MAX_BULLETS - player->GetNumOfBullets(), bullets);
        player->AddAmmo(bulletsTaken,grenadesTaken);
        bullets -= bulletsTaken;
        grenades -= grenadesTaken;
        if(bullets <= 0 || grenades <= 0){
            alive = false;
        }
    }

    int GetNumOfGrenades()const{return grenades;}
    int GetNumOfBullets()const{return bullets;}

protected:
    void _Tick(vector<Entity *> &entities) override {}

private:
    int bullets = AMMO_BOX_BULLETS;
    int grenades = AMMO_BOX_GRENADES;
};

#endif //TEST_AMMOBOX_H
