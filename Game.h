//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_GAME_H
#define TEST_GAME_H

#include <vector>
#include "HealthBox.h"
#include "AmmoBox.h"
#include "Player.h"
#include "Wall.h"
#include "Path.h"
#include "Fighter.h"

const int GAME_HEIGHT = 140;
const int GAME_WIDTH = 140;

const int GAME_SECURITY_SIMULATIONS = 10000;

const int GAME_MAX_SPEED = 10;

const int GAME_NUM_OF_ROOMS = 12;

const int GAME_NUM_OF_FIGHTERS = 2;
const int GAME_NUM_OF_SUPPORTERS = 1;

using namespace std;

class Room;

class Cell{
public:
    typedef enum{
        White,
        Grey,
        Target,
        Origin,
    }Cell_Type_t;

    Cell(Cell *parent, Cell_Type_t type, const Point2D& location);
    Cell(Cell *parent, Cell_Type_t type);

    Cell *getParent() const;

    Cell_Type_t getType() const;

    const Point2D &getLocation() const;

    int GetLength()const{return length;}

private:
    Cell* parent;
    Cell_Type_t type;
    Point2D location;
    int length = 0;
};

class Game {
public:
    typedef Entity *Board_t[GAME_WIDTH][GAME_HEIGHT];
    typedef Cell* Board_Cells_t[GAME_WIDTH][GAME_HEIGHT];
    typedef double Board_Security_t[GAME_WIDTH][GAME_HEIGHT];

    static Game& GetInstance(){
        static Game* instance = nullptr;
        if(instance == nullptr)
            instance = new Game();
        return *instance;
    }
    void Init();
    vector<Entity*>& GetEntities(){return entities;}
    vector<Entity*>& GetShootings(){return shootings;}
    int Tick();

    bool IsRunning() const{return running;}

    vector<Player*> GetTeamMembers(int team);
    vector<Player*> GetTeamMembers(const Player* player);
    vector<Player*> GetEnemyTeam(int myTeam);
    vector<Fighter*> GetTeamFighters(int team);

    template<typename T>
    vector<T> GetEntitiesOfType() {
        vector<T> res;
        for(auto* entity : entities){
            T t = dynamic_cast<T>(entity);
            if(t)
                res.push_back(t);
        }
        return res;
    }

    template<typename T>
    vector<T> GetEntitiesOfType(function<bool(T)> predicate){
        vector<T> res;
        for(auto* entity : entities){
            T t = dynamic_cast<T>(entity);
            if(t && predicate(t))
                res.push_back(t);
        }
        return res;
    }

    vector<Room>& GetRooms(){return rooms;}

    long long Ticks() const{return ticks;}

    void CleanBoardCells(){
        for(auto & i : BOARD_CELLS){
            for(auto & j : i){
                if(j) {
                    delete j;
                    j = nullptr;
                }
            }
        }
    }

    static bool WithinBoundaries(int x, int y){
        return (x >= 0 && x < GAME_WIDTH) && (y >= 0 && y < GAME_HEIGHT);
    }

    Board_Cells_t  BOARD_CELLS = {0};
    Board_t BOARD = {nullptr};
    Board_Security_t  BOARD_SECURITY = {0};
    static Wall* CONST_WALL;
    static Path* CONST_PATH;
private:
    void InitRooms();
    void GenerateBoard();
    static void SimulateSecurityMap();
    bool running = true;
    Game();
    void Tick(vector<Entity*>& ent);
    void UpdateLocations(vector<Entity*>& ent);
    vector<Entity*> entities;
    vector<Entity*> shootings;
    long long ticks = 0;
    vector<Room> rooms;
};

class Room{
public:
    Room(const Point2D& center, int width, int height): center(center),width(width),height(height), safestPlace(center){}
    Room(): Room(Point2D(0,0),1,1){}
    const Point2D &GetCenter() const {
        return center;
    }

    int GetWidth() const {
        return width;
    }

    int GetHeight() const {
        return height;
    }

    const Point2D &GetSafestPlace()const{return safestPlace;}

    void SetSafestPlace(const Point2D& point){safestPlace = point;}

    Point2D GetRandomPoint() const;
    double Distance(const Room& room)const;
    bool Overlap(const Room& room)const;
    bool Overlap()const;
    bool Contains(const Point2D& point)const;
private:
    Point2D center;
    int width,height;
    Point2D safestPlace;
};

#endif //TEST_GAME_H
