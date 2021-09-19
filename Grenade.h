//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_GRENADE_H
#define TEST_GRENADE_H


#include "Entity.h"
#include "Bullet.h"
#include "AudioManager.h"
#include <cmath>

const int GRENADE_NUM_OF_BULLETS = 16;
const int GRENADE_SPEED = 2;
const int GRENADE_MAX_DISTANCE = 25;

class Grenade : public Entity{
public:
    explicit Grenade(int x, int y, const Point2D& target, int team) :
    Entity(x,y,GRENADE_SPEED,team), target(target),
    explosionAudioSource(AudioManager::GetInstance().CreateSource(location,"../audio/explosion.wav")){
        UpdateAngle();
        explosionAudioSource.SetVolume(0.15f);
    }
    void UpdateAngle(){
        angle = Point2D::AngleBetweenPoints(location,target);
    }
    bool PlayingSound()override;
protected:
    void _Tick(vector<Entity *> &entities) override;
private:
    AudioSource explosionAudioSource;
    void Explode();
    Point2D target;
    double angle = 0;
};


#endif //TEST_GRENADE_H
