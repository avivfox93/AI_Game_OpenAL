//
// Created by Aviv on 29/06/2021.
//

#include "Player.h"
#include "Wall.h"
#include "Game.h"
#include "AStar.h"
#include <cmath>

static const int CRITICAL_HEALTH = 15;
static const int MEDIUM_HEALTH = 50;
static const int FULL_HEALTH = 100;

auto GetPredicateHealthBox(){
    return [](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }if(dynamic_cast<HealthBox*>(entity)){
        return Cell::Target;
    }
        return Cell::White;
    };
}

auto GetCompareHealthBoxes(const vector<HealthBox*>& healthBoxes){
    return [healthBoxes](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aMinDistance = 0, bMinDistance = 0;
        for(auto& box : healthBoxes){
            double distance = box->GetLocation().ManhattanDistance(aLocation);
            if(distance < aMinDistance){
                aMinDistance = distance;
            }
            distance = box->GetLocation().ManhattanDistance(bLocation);
            if(distance < bMinDistance){
                bMinDistance = distance;
            }
        }
        return (aMinDistance + a->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)a->getLocation().x][(int)a->getLocation().y]+1) >
               (bMinDistance + b->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)b->getLocation().x][(int)b->getLocation().y]+1);
    };
}

Player::Player(int x, int y, int team, PlayerState_t startState) : Entity(x,y,PLAYER_SPEED,team), stateMachine(this,startState){
    AddHandler(COLLECTING_HEALTH, HandelCollectingHealth);
}

void Player::_Tick(vector<Entity*>& entities) {
    stateMachine.Tick();
    HandelState(stateMachine.GetState());
}

void Player::HandelState(Player::PlayerState_t state){
    auto a = handlers.find(state);
    if(a != handlers.end()){
        a->second(this);
    }
}

bool Player::ClearSight(const Point2D& a, const Point2D& b) {
    double angle = round(Point2D::AngleBetweenPoints(a,b));
    Point2D nextLocation = a;
    while(nextLocation.Distance(b) >= 1.0) {
        nextLocation += Point2D(cos(angle), sin(angle));
        auto* wall = dynamic_cast<Wall*>(Game::GetInstance().BOARD[(int)round(nextLocation.x)][(int)round(nextLocation.y)]);
        if(wall){
            return false;
        }
    }
    return true;
}

Player &Player::operator+=(HealthBox &box) {
    box.GiveHealth(this);
    return *this;
}

void Player::HandelCollectingHealth(Player *player) {
    auto healthBoxes = Game::GetInstance().GetEntitiesOfType<HealthBox*>();

    for(auto& healthBox : healthBoxes){
        if(player->GetLocation().Distance(healthBox->GetLocation()) <= 1){
            healthBox->GiveHealth(player);
            return;
        }
    }

    auto Compare = GetCompareHealthBoxes(healthBoxes);
    auto Predicate = GetPredicateHealthBox();
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}
