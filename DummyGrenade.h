//
// Created by Aviv on 08/07/2021.
//

#ifndef TEST_DUMMYGRENADE_H
#define TEST_DUMMYGRENADE_H

#include "Point2D.h"
#include "Game.h"

class DummyBullet{
public:
    DummyBullet(const Point2D& location, double angle, double power): location(location),angle(angle),power(power){}
    void Simulate();
private:
    Point2D location;
    double angle;
    double power;
};

class DummyGrenade {
public:
    explicit DummyGrenade(const Point2D& location, double bulletPower): location(location), bulletPower(bulletPower){}
    void Simulate();
private:
    Point2D location;
    double bulletPower;
};

#endif //TEST_DUMMYGRENADE_H
