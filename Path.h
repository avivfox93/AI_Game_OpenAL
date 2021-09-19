//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_PATH_H
#define TEST_PATH_H

#include "Entity.h"

class Path : public Entity{
public:
    explicit Path(int x = 0, int y = 0) : Entity(x,y){}
    void UpdateLocation() override {}

protected:
    void _Tick(vector<Entity *> &entities) override {}
};

#endif //TEST_PATH_H
