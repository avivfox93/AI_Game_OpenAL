//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_PLAYER_H
#define TEST_PLAYER_H


#include "StateMachine.h"
#include "Point2D.h"
#include "Entity.h"
#include <map>
#include <utility>

const int PLAYER_SPEED = 1;
const int PLAYER_MAX_HEALTH = 100;
const int PLAYER_CRITICAL_HELATH = 30;

const int PLAYER_MAX_GRENADES = 5;
const int PLAYER_MAX_BULLETS = 5;

using namespace std;

class HealthBox;

class Player : public Entity{
public:
    typedef enum{
        SEARCHING,
        FOLLOWING,
        HIDING,
        COLLECTING_AMMO,
        GIVING_AMMO,
        COLLECTING_HEALTH,
    }PlayerState_t;
    explicit Player(int x = 0, int y = 0, int team = 0, PlayerState_t startState = SEARCHING);
    void Hit(int power){ health -= power; alive = health > 0;}
    int GetHealth() const {return health;}
    int GetNumOfBullets() const {return bullets;}
    int GetNumOfGrenades() const {return grenades;}
    void AddAmmo(int addBullets, int addGrenades){this->bullets+=addBullets; this->grenades+=addGrenades;}
    void TakeAmmo(int takeBullets, int takeGrenades){this->bullets-=takeBullets; this->grenades-=takeGrenades;}
    bool NeedAmmo() const {return bullets < PLAYER_MAX_BULLETS/2 || grenades < PLAYER_MAX_GRENADES/2;}
    void SetNextLocation(const Point2D& location){nextLocation = location;}
    Player& operator+=(HealthBox& box);
    static void HandelCollectingHealth(Player *player);
    static bool ClearSight(const Point2D& a, const Point2D& b);
protected:
    void AddHandler(PlayerState_t state, function<void(Player*)> handler){handlers[state] = std::move(handler);}
    void HandelState(PlayerState_t state);
    StateMachine<Player,Player::PlayerState_t> stateMachine;
    void _Tick(vector<Entity*>& entities) override;
    int health = PLAYER_MAX_HEALTH;
    int bullets = PLAYER_MAX_BULLETS;
    int grenades = PLAYER_MAX_GRENADES;

private:
    map<PlayerState_t,function<void(Player*)>> handlers;
};


#endif //TEST_PLAYER_H
