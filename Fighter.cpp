//
// Created by Aviv on 29/06/2021.
//

#include <iostream>
#include "Fighter.h"
#include "Grenade.h"
#include "Game.h"
#include "AStar.h"

auto GetPredicateHiding(int team){
    return [team](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }
        for(auto& room : Game::GetInstance().GetRooms()){
            bool roomContainsEnemy = false;
            for(auto& enemy : Game::GetInstance().GetEnemyTeam(team)){
                if(room.Contains(enemy->GetLocation()))
                    roomContainsEnemy = true;
            }
            if(room.GetSafestPlace().Distance(Point2D(x,y)) < 1 && !roomContainsEnemy)
                return Cell::Target;
        }
        return Cell::White;
    };
}

auto GetCompareHiding(const vector<Player*>& enemies){
    return [enemies](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aDistanceToRoom = 0, bDistanceToRoom = 0;;
        for(auto& room : Game::GetInstance().GetRooms()){
            double extra = 0;
            for(auto& enemy : enemies){
                extra += enemy->GetLocation().Distance(room.GetSafestPlace());
            }
            double distance = aLocation.Distance(room.GetSafestPlace()) + extra;
            if(distance > aDistanceToRoom){
                aDistanceToRoom = distance;
            }
            distance = bLocation.Distance(room.GetSafestPlace()) + extra;
            if(distance > bDistanceToRoom){
                bDistanceToRoom = distance;
            }
        }
        return (aDistanceToRoom + a->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)a->getLocation().x][(int)a->getLocation().y]+1) >
               (bDistanceToRoom + b->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)b->getLocation().x][(int)b->getLocation().y]+1);
    };
}

auto GetPredicateSearching(const vector<Player*>& enemies, int team){
    return [enemies, team](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        Point2D loc(x,y);
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }
        for(auto& enemy : Game::GetInstance().GetEnemyTeam(team)){
            double distance = loc.Distance(enemy->GetLocation());
            if(distance > 5 && distance < 10 && Player::ClearSight(loc,enemy->GetLocation()))
                return Cell::Target;
        }
        return Cell::White;
    };
}

auto GetCompareSearching(const vector<Player*>& enemies){
    return [enemies](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aMinDistance = 0, bMinDistance = 0;
        for(auto& enemy : enemies){
            double distance = enemy->GetLocation().ManhattanDistance(aLocation);
            if(distance < aMinDistance){
                aMinDistance = distance;
            }
            distance = enemy->GetLocation().ManhattanDistance(bLocation);
            if(distance < bMinDistance){
                bMinDistance = distance;
            }
        }
        return (aMinDistance + a->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)a->getLocation().x][(int)a->getLocation().y]+1) >
               (bMinDistance + b->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)b->getLocation().x][(int)b->getLocation().y]+1);
    };
}

bool Fighter::ThrowGrenade(const Point2D &target) {

    if (grenades <= 0)
        return false;
    auto grenade = new Grenade((int) location.x, (int) location.y, target, team);
    Game::GetInstance().GetShootings().push_back(grenade);
//    std::cout << "Grenade!" << std::endl;
    --grenades;
    lastGrenadeTicks = Game::GetInstance().Ticks();
    throwingSoundSource.Play();
    return true;
}

bool Fighter::ShootBullet(const Point2D &target) {
    if (bullets <= 0)
        return false;
    double angle = Point2D::AngleBetweenPoints(location, target);
    auto bullet = new Bullet((int) location.x, (int) location.y, team);
    bullet->SetAngle(angle);
    Game::GetInstance().GetShootings().push_back(bullet);
//    std::cout << "Shoot!" << std::endl;
    --bullets;
    lastBulletTicks = Game::GetInstance().Ticks();
    firingSoundSource.Play();
    return true;
}

bool Fighter::CanShoot() const {
    return bullets > 0 && (Game::GetInstance().Ticks() - lastBulletTicks > TICKS_BETWEEN_BULLETS);
}

bool Fighter::CanThrowGrenade(const Point2D &target) const {
    return CanThrowGrenade() && (location.Distance(target) <= GRENADE_MAX_DISTANCE);
}

bool Fighter::CanThrowGrenade() const {
    return grenades > 0 && (Game::GetInstance().Ticks() - lastGrenadeTicks > TICKS_BETWEEN_GRENADES);
}

Fighter::Fighter(int x, int y, int team) : Player(x, y, team),
                                           firingSoundSource(AudioManager::GetInstance().CreateSource(location,"../audio/gun-shot.wav")),
                                           throwingSoundSource(AudioManager::GetInstance().CreateSource(location,"../audio/grenade-throw.wav")){
    stateMachine.AddTransition(SEARCHING,HIDING,SearchingToHiding);
    stateMachine.AddTransition(HIDING,SEARCHING,HidingToSearching);
    stateMachine.AddTransition(HIDING,COLLECTING_HEALTH,HidingToCollectingHealth);
    stateMachine.AddTransition(COLLECTING_HEALTH,HIDING,CollectingHealthToHiding);

    AddHandler(SEARCHING, HandelSearching);
    AddHandler(HIDING, HandelHiding);
}

void Fighter::HandelSearching(Player *player) {
    auto *fighter = dynamic_cast<Fighter *>(player);
    for (auto *enemy : Game::GetInstance().GetEntitiesOfType<Player *>(
            [player](Player *t) {
                return t->GetTeam() != player->GetTeam();
            })) {
        if (ClearSight(player->GetLocation(), enemy->GetLocation())) {
            if (fighter->CanShoot()) {
                fighter->ShootBullet(enemy->GetLocation());
            }else if (fighter->CanThrowGrenade(enemy->GetLocation())) {
                Point2D grenadeTarget(0,0);
                do{
                    grenadeTarget = Point2D::GetRandomInRadius(enemy->GetLocation(), 5);
                }while(!dynamic_cast<Path*>(Game::GetInstance().BOARD[(int)grenadeTarget.x][(int)grenadeTarget.y]));
                fighter->ThrowGrenade(grenadeTarget);
            }
            return;
        }
    }
    auto enemies =Game::GetInstance().GetEntitiesOfType<Player*>([player](Player* p){
        return player->GetTeam() != p->GetTeam();
    });
    auto Compare = GetCompareSearching(enemies);
    int team = player->GetTeam();
    auto Predicate = GetPredicateSearching(enemies,team);
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}

void Fighter::HandelHiding(Player *player) {
    auto enemies =Game::GetInstance().GetEntitiesOfType<Player*>([player](Player* p){
        return player->GetTeam() != p->GetTeam();
    });
    auto Compare = GetCompareHiding(enemies);
    auto Predicate = GetPredicateHiding(player->GetTeam());
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}

bool Fighter::SearchingToHiding(Player *player) {
    auto *fighter = dynamic_cast<Fighter *>(player);
    if (!fighter) return false;
    bool healthBoxes = !Game::GetInstance().GetEntitiesOfType<HealthBox *>().empty();
    bool res = (fighter->GetHealth() < PLAYER_CRITICAL_HELATH && healthBoxes) || !(fighter->CanShoot() || fighter->CanThrowGrenade());
    return res;
}

bool Fighter::HidingToSearching(Player *player) {
    return !SearchingToHiding(player);
}

bool Fighter::HidingToCollectingHealth(Player *player) {
    bool healthBoxes = !Game::GetInstance().GetEntitiesOfType<HealthBox *>().empty();
    return player->GetHealth() < PLAYER_MAX_HEALTH / 2 && healthBoxes;
}

bool Fighter::CollectingHealthToHiding(Player *player) {
    bool healthBoxes = !Game::GetInstance().GetEntitiesOfType<HealthBox *>().empty();
    return (player->GetHealth() >= (int) (PLAYER_MAX_HEALTH * 0.9f)) || !healthBoxes;
}


bool Fighter::PlayingSound() {
    return firingSoundSource.Playing() || throwingSoundSource.Playing();
}
