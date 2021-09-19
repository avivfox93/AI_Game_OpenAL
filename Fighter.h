//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_FIGHTER_H
#define TEST_FIGHTER_H


#include "Player.h"
#include "AudioManager.h"

const int TICKS_BETWEEN_BULLETS = 50;
const int TICKS_BETWEEN_GRENADES = 200;

class Fighter : public Player{
public:
    explicit Fighter(int x = 0, int y = 0, int team = 0);
    bool ThrowGrenade(const Point2D& target);
    bool ShootBullet(const Point2D& target);
    bool CanShoot() const;
    bool CanThrowGrenade(const Point2D& target) const;
    bool CanThrowGrenade() const;

    bool PlayingSound() override;

private:
    static void HandelSearching(Player* player);
    static void HandelHiding(Player* player);
    static void HandelCollectingHealth(Player* player);

    static bool SearchingToHiding(Player* player);
    static bool HidingToSearching(Player* player);
    static bool HidingToCollectingHealth(Player* player);
    static bool CollectingHealthToHiding(Player* player);

    AudioSource firingSoundSource;
    AudioSource throwingSoundSource;
    long long lastBulletTicks = 0;
    long long lastGrenadeTicks = 0;
};


#endif //TEST_FIGHTER_H
