//
// Created by Aviv on 30/06/2021.
//

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "Player.h"
#include "AmmoBox.h"

class Support : public Player{
public:
    explicit Support(int x = 0, int y = 0, int team = 0);
    Support& operator+=(AmmoBox& box);
    int GrenadesNeeded(const vector<AmmoBox*>& ammoBoxes)const;
    int BulletsNeeded(const vector<AmmoBox*>& ammoBoxes)const;
protected:
    void _Tick(vector<Entity *> &entities) override;

private:
    static bool FollowingToCollectingHealth(Player* player);
    static bool FollowingToCollectingAmmo(Player* player);
    static bool FollowingToGivingAmmo(Player* player);
    static bool CollectingHealthToFollowing(Player* player);
    static bool CollectingAmmoToFollowing(Player* player);
    static bool GivingAmmoToFollowing(Player* player);
    static bool CollectingAmmoToCollectingHealth(Player* player);

    static void HandelFollowing(Player* player);
    static void HandelCollectingAmmo(Player* player);
    static void HandelGivingAmmo(Player* player);
};


#endif //TEST_SUPPORT_H
