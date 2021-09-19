//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_BULLET_H
#define TEST_BULLET_H


#include "Entity.h"
#include "AudioManager.h"

static const int BULLET_SPEED = 2;
static const int BULLET_POWER = 8;

using namespace std;

class Bullet : public Entity{
public:
    explicit Bullet(int x = 0, int y = 0, int team = 0, int power = BULLET_POWER) :
        Entity(x,y,BULLET_SPEED,team),speed(speed),power(power),
        bulletHitSoundSource(AudioManager::GetInstance().CreateSource(location,"../audio/bullet-impact.wav")){
        bulletHitSoundSource.SetVolume(0.15);
    }
    void SetAngle(double angle){this->angle = angle;}
    void EnableSound(bool sound){this->sound = sound;}
    bool PlayingSound()override;
protected:
    void _Tick(vector<Entity *> &entities) override;
private:
    AudioSource bulletHitSoundSource;
    int power;
    double angle = 0;
    double speed;
    bool sound = true;
};


#endif //TEST_BULLET_H
