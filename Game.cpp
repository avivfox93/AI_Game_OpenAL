//
// Created by Aviv on 29/06/2021.
//

#include "Game.h"
#include "Random.h"
#include "DummyGrenade.h"
#include "Support.h"
#include "AStar.h"
#include "AudioManager.h"
#include<bits/stdc++.h>

Wall* Game::CONST_WALL = nullptr;
Path* Game::CONST_PATH = nullptr;

int Game::Tick() {
    if(GetTeamFighters(0).empty()){
//        printf("Team 2 Won!\n");
        return 2;
    }else if(GetTeamFighters(1).empty()){
//        printf("Team 1 Won!\n");
        return 1;
    }
    Tick(entities);
    Tick(shootings);

    UpdateLocations(entities);
    UpdateLocations(shootings);

    ++ticks;
    return 0;
}

void Game::Init() {
    GenerateBoard();
    InitRooms();
    AudioManager::GetInstance().Init();
    for(int i = 0 ; i < GAME_WIDTH ; i++){
        BOARD[i][0] = CONST_WALL;
        BOARD[i][GAME_HEIGHT-1] = CONST_WALL;
    }
    for(int i = 0 ; i < GAME_HEIGHT ; i++){
        BOARD[0][i] = CONST_WALL;
        BOARD[GAME_WIDTH-1][i] = CONST_WALL;
    }
    for(int i = 0 ; i < 15 ; i++)
        SimulateSecurityMap();
    for(int team = 0 ; team < 2 ; team++){
        Room room;
        if(team == 0){
            int ran = (int)Random::Rand(0,(double)rooms.size()-1);
            room = rooms[ran];
        }else{
            double max_dist = 0;
            for(auto& r : rooms){
                double roomDistance = r.Distance(room);
                if(roomDistance > max_dist){
                    max_dist = roomDistance;
                    room = r;
                }
            }
        }
        for(int i = 0 ; i < GAME_NUM_OF_FIGHTERS ; i++){
            Point2D loc = room.GetRandomPoint();
            auto* fighter = new Fighter((int)loc.x,(int)loc.y,team);
            entities.push_back(fighter);
        }
        for(int i = 0 ; i < GAME_NUM_OF_SUPPORTERS ; i++){
            Point2D loc = room.GetRandomPoint();
            auto* support = new Support((int)loc.x,(int)loc.y,team);
            entities.push_back(support);
        }
    }
    for(Entity* e : entities){
        BOARD[(int) e->GetLocation().x][(int) e->GetLocation().y] = e;
    }
    for(auto & room : rooms){
        if(Random::Rand(0,10) < 5) continue;
        int width = (int)room.GetWidth();
        int height = (int)room.GetHeight();
        int centerX = (int)room.GetCenter().x;
        int centerY = (int)room.GetCenter().y;
        double safestValue = BOARD_SECURITY[centerX][centerY];
        Point2D result(0,0);
        for(int x = centerX - width/2 ; x < centerX + width/2 ; x++){
            for(int y = centerY - height/2 ; y < centerY + height/2 ; y++){
                if(BOARD_SECURITY[x][y] < safestValue){
                    safestValue = BOARD_SECURITY[x][y];
                    result.x = x;
                    result.y = y;
                }
            }
        }
        room.SetSafestPlace(result);
    }
}

vector<Player*> Game::GetTeamMembers(int team){
    vector<Player*> result;
    for(auto* e : entities){
        auto* p = dynamic_cast<Player*>(e);
        if(p && p->GetTeam() == team){
            result.push_back(p);
        }
    }
    return result;
}

vector<Player*> Game::GetTeamMembers(const Player* player){
    vector<Player*> result;
    for(auto* e : entities){
        auto* p = dynamic_cast<Player*>(e);
        if(p && (p != player) && (p->GetTeam() == player->GetTeam())){
            result.push_back(p);
        }
    }
    return result;
}

vector<Player*> Game::GetEnemyTeam(int myTeam){
    return GetEntitiesOfType<Player*>([myTeam](Player* p){return p->GetTeam() != myTeam;});
}

vector<Fighter*> Game::GetTeamFighters(int team){
    return GetEntitiesOfType<Fighter*>([team](Fighter* p){return p->GetTeam() == team;});
}

void Game::Tick(vector<Entity*>& ent) {
    vector<Entity*> toDelete;
    for(Entity* e : ent){
        if(e->IsAlive()) {
            e->Tick(ent);
        }else{
            BOARD[(int)round(e->GetLocation().x)][(int)round(e->GetLocation().y)] = CONST_PATH;
            if(!e->PlayingSound()){
                toDelete.push_back(e);
            }
        }
    }
    for(Entity* e : toDelete){
        auto f = std::find(ent.begin(),ent.end(),e);
        if(f != ent.end()){
            ent.erase(f);
            BOARD[(int)round(e->GetLocation().x)][(int)round(e->GetLocation().y)] = CONST_PATH;
            delete e;
        }
    }
}

void Game::UpdateLocations(vector<Entity *> &ent) {
    for(Entity* e : ent){
        if(!e->IsAlive())
            continue;
        if(BOARD[(int) round(e->GetLocation().x)][(int) round(e->GetLocation().y)] == e)
            BOARD[(int) round(e->GetLocation().x)][(int) round(e->GetLocation().y)] = CONST_PATH;
        e->UpdateLocation();
        BOARD[(int) round(e->GetLocation().x)][(int) round(e->GetLocation().y)] = e;
    }
}

void Game::SimulateSecurityMap() {
    double power = 1.0/GAME_SECURITY_SIMULATIONS;
    Game& game = Game::GetInstance();
    for(int i = 0 ; i < GAME_SECURITY_SIMULATIONS ; i++){
        int x,y;
        do{
            x = (int)Random::Rand(0,GAME_WIDTH);
            y = (int)Random::Rand(0,GAME_HEIGHT);
        }while(!dynamic_cast<Path*>(game.BOARD[x][y]));
        Point2D loc(x,y);
        auto grenade = DummyGrenade(loc,power);
        grenade.Simulate();
    }
}

void Game::GenerateBoard() {
    for(auto & x : BOARD){
        for(auto & y : x){
                y = CONST_WALL;
        }
    }
}

void Game::InitRooms() {
    const double minW = 10, minH = 10;
    for(int i = 0 ; i < GAME_NUM_OF_ROOMS ; i++){
        Room room;
        do {
            int x = (int) Random::Rand(8 + minW / 2, GAME_WIDTH - minW - 16 + 8 + minW / 2);
            int y = (int) Random::Rand(8 + minH / 2, GAME_HEIGHT - minH - 16 + 8 + minH / 2);

            int width = (int) Random::Rand(minW, minW - 2 + 20);
            int height = (int) Random::Rand(minH, minH - 2 + 20);

            auto center = Point2D(x, y);
            room = Room(center, width, height);
        }while(room.Overlap());
        rooms.push_back(room);
        for(int i = 0 ; i < room.GetWidth() ; i++){
            for(int j = 0 ; j < room.GetHeight() ; j++){
                BOARD[(int)room.GetCenter().x + i - room.GetWidth()/2][(int)room.GetCenter().y + j - room.GetHeight()/2] = CONST_PATH;
            }
        }
    }

    for(int i = 0 ; i < rooms.size() ; i++) {
        double minDistance = 0;
        int index = 0;
        for(int j = i ; j < rooms.size() ; j++){
            if(i == j) continue;
            double dist = rooms[i].Distance(rooms[j]);
            if(dist < minDistance){
                minDistance = dist;
            }
        }
        auto& from = rooms[i];
        auto& to = rooms[index];
        auto Compare = [to](Cell* a,Cell* b){
                double aDistance = a->getLocation().ManhattanDistance(to.GetCenter());
                double bDistance = b->getLocation().ManhattanDistance(to.GetCenter());

            return (aDistance + a->GetLength()) > (bDistance + b->GetLength());
        };
        auto Predicate = [to](int x, int y){
            Point2D location(x,y);
            if(location.Distance(to.GetCenter()) <= 1)
                return Cell::Target;
            return Cell::White;
        };
        AStar<decltype(Compare), decltype(Predicate)> aStar(from.GetCenter(), Compare, Predicate);
        Point2D res(0,0);
        if(aStar.Run(res)){
//            printf("Paving way\n");
            Cell* current = BOARD_CELLS[(int)to.GetCenter().x][(int)to.GetCenter().y];
            while(current){
                BOARD[(int)current->getLocation().x+1][(int)current->getLocation().y] = CONST_PATH;
                BOARD[(int)current->getLocation().x][(int)current->getLocation().y+1] = CONST_PATH;
//                BOARD[(int)current->getLocation().x][(int)current->getLocation().y-1] = CONST_PATH;
//                BOARD[(int)current->getLocation().x-1][(int)current->getLocation().y] = CONST_PATH;
                current = current->getParent();
            }
        }
    }
    for(auto& room : rooms){
        Point2D point = room.GetRandomPoint();
        auto* healthBox = new HealthBox((int)std::round(point.x),(int)std::round(point.y));
        entities.push_back(healthBox);
        BOARD[(int)std::round(point.x)][(int)std::round(point.y)] = healthBox;
        point = room.GetRandomPoint();
        auto* ammoBox = new AmmoBox((int)std::round(point.x),(int)std::round(point.y));
        entities.push_back(ammoBox);
        BOARD[(int)std::round(point.x)][(int)std::round(point.y)] = ammoBox;
    }
}

Game::Game() {
    CONST_WALL = new Wall();
    CONST_PATH = new Path();
}

bool Room::Overlap(const Room& room) const {
    int m_x = (int)center.x;
    int m_y = (int)center.y;
    int r_x = (int)room.GetCenter().x;
    int r_y = (int)room.GetCenter().y;
    return m_x < r_x + room.GetWidth() && m_x + width > r_x && m_y < r_y + room.GetHeight() && m_y + height > r_y;
}

bool Room::Overlap() const {
    for(int i = 0 ; i < width + 4 ; i++){
        for(int j = 0 ; j < height + 4 ; j++){
            if(dynamic_cast<Path*>(Game::GetInstance().BOARD[(int)center.x + i - 2 - width/2][(int)center.y + j - 2 - height/2])){
                return true;
            }
        }
    }
    return false;
}

bool Room::Contains(const Point2D& point)const{
    int startX = (int)center.x-width/2;
    int startY = (int)center.y-height/2;
    int endX = (int)center.x+width/2;
    int endY = (int)center.y+height/2;
    return point.x >= startX && point.x <= endX && point.y >= startY && point.y <= endY;
}

double Room::Distance(const Room& room) const {
    return center.Distance(room.GetCenter());
}

Point2D Room::GetRandomPoint() const {
    auto& game = Game::GetInstance();
    int x,y;
    do{
        x = (int)Random::Rand(center.x-(double)width/2,center.x+(double)width/2);
        y = (int)Random::Rand(center.y-(double)height/2,center.y+(double)height/2);
    }while(!dynamic_cast<Path*>(game.BOARD[x][y]));
    return {(double)x, (double)y};
}

Cell::Cell(Cell *parent, Cell::Cell_Type_t type, const Point2D &location) : parent(parent),type(type), location(location) {
    if(parent){
        length = parent->GetLength() + 1;
    }
}

Cell *Cell::getParent() const {return parent;}

Cell::Cell_Type_t Cell::getType() const {return type;}

const Point2D &Cell::getLocation() const {return location;}

Cell::Cell(Cell *parent, Cell::Cell_Type_t type) : Cell(parent,type,Point2D(0,0)){}
