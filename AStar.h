//
// Created by Aviv on 03/07/2021.
//

#ifndef TEST_ASTAR_H
#define TEST_ASTAR_H

#include <functional>
#include <utility>
#include "Point2D.h"
#include "Game.h"
#include <queue>
#include "Player.h"
#include <cmath>
#include <stdio.h>

using namespace std;
template<typename CMP,typename PREDICATE>
class AStar{
public:
    template<typename CMP_FUNC>
    explicit AStar(Point2D location, CMP_FUNC comp, PREDICATE predicate):
        location(location), game(Game::GetInstance()), queue(comp), predicate(predicate){}

    bool Run(Point2D& result){
        game.CleanBoardCells();
        game.BOARD_CELLS[(int)round(location.x)][(int)round(location.y)] = new Cell(nullptr,Cell::Origin,location);
        queue.push(game.BOARD_CELLS[(int)round(location.x)][(int)round(location.y)]);
        running = true;
        while(running){
            if(Tick() == false) {
                printf("No PATH!\n");
                running = false;
                return false;
            }
        }
        result = RecoverPath();
        return true;
    }

private:
    bool Tick(){
        if(predicate((int)location.x,(int)location.y) == Cell::Target){
            running = false;
            return true;
        }
        if(queue.empty())
            return false;
        location = queue.top()->getLocation();
        queue.pop();
        CheckNeighbors();
        return true;
    }
    void CheckNeighbors(){
        CheckCell((int)location.x+1,(int)location.y);
        CheckCell((int)location.x,(int)location.y+1);
        CheckCell((int)location.x-1,(int)location.y);
        CheckCell((int)location.x,(int)location.y-1);
    }
    void CheckCell(int x, int y){
        if(!game.WithinBoundaries(x,y))
            return;
        if(game.BOARD_CELLS[x][y])
            return;
        Cell::Cell_Type_t type = predicate(x,y);
        game.BOARD_CELLS[x][y] = new Cell(game.BOARD_CELLS[(int)round(location.x)][(int)round(location.y)],type,Point2D(x,y));
        switch(type){
            case Cell::Target:
            case Cell::White:
                queue.push(game.BOARD_CELLS[x][y]);
                break;
            default:
                break;
        }
    }
private:
    Point2D RecoverPath(){
        Cell* current = game.BOARD_CELLS[(int)round(location.x)][(int)round(location.y)];
        Cell* next = current->getParent();
        if(!next)
            return current->getLocation();
        while(next->getParent()){
            Cell* temp = next;
            next = current->getParent();
            current = temp;
        }
        return current->getLocation();
    }
    Point2D location;
    priority_queue<Cell*,vector<Cell*>,CMP> queue;
    bool running = true;
    PREDICATE predicate;
    Game& game;
};

#endif //TEST_ASTAR_H
