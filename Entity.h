//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_ENTITY_H
#define TEST_ENTITY_H


#include "Point2D.h"
#include <vector>

using namespace std;

class Entity {
public:
    explicit Entity(int x = 0, int y = 0, int speed = 1, int team = 0) : nextLocation(x,y), location(x,y), speed(speed), team(team) {}
    virtual void Tick(vector<Entity*>& entities);
    virtual void UpdateLocation();
    virtual bool PlayingSound(){return false;}
    bool IsAlive()const{return alive;}
    Point2D& GetLocation(){return location;}
    const Point2D& GetLocation()const{return location;}
    int GetTeam() const {return team;}
    virtual ~Entity()= default;
protected:
    virtual void _Tick(vector<Entity*>& entities) = 0;
    Point2D nextLocation;
    Point2D location;
    bool alive = true;
    int speed;
    int team;
    long long nextTick = 0;
};


#endif //TEST_ENTITY_H
