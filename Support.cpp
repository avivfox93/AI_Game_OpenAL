//
// Created by Aviv on 30/06/2021.
//

#include "Support.h"
#include "HealthBox.h"
#include "Game.h"
#include "AStar.h"

auto GetPredicateCollectingAmmo(){
    return [](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }if(dynamic_cast<AmmoBox*>(entity)){
            return Cell::Target;
        }
        return Cell::White;
    };
}

auto GetCompareCollectingAmmo(const vector<AmmoBox*>& ammoBoxes){
    return [ammoBoxes](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aMinDistance = 0, bMinDistance = 0;
        for(auto& box : ammoBoxes){
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

auto GetPredicateFollowing(Player* player){
    return [player](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        Point2D loc(x,y);
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }
        for(auto& member : Game::GetInstance().GetTeamMembers(player)){
            double distance = loc.Distance(member->GetLocation());
            if(distance < 10 && distance > 4)
                return Cell::Target;
        }
        return Cell::White;
    };
}

auto GetCompareFollowing(const vector<Player*>& teamMembers){
    return [teamMembers](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aMinDistance = 0, bMinDistance = 0;
        for(auto& member : teamMembers){
            if(!member->NeedAmmo())
                continue;
            double distance = member->GetLocation().ManhattanDistance(aLocation);
            if(distance < aMinDistance){
                aMinDistance = distance;
            }
            distance = member->GetLocation().ManhattanDistance(bLocation);
            if(distance < bMinDistance){
                bMinDistance = distance;
            }
        }
        return (aMinDistance + a->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)a->getLocation().x][(int)a->getLocation().y]+1) >
               (bMinDistance + b->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)b->getLocation().x][(int)b->getLocation().y]+1);
    };
}

auto GetPredicateGivingAmmo(Player* player){
    return [player](int x, int y){
        Entity* entity = Game::GetInstance().BOARD[x][y];
        Point2D loc(x,y);
        if(dynamic_cast<Wall*>(entity)){
            return Cell::Grey;
        }
        for(auto& member : Game::GetInstance().GetTeamMembers(player)){
            double distance = loc.Distance(member->GetLocation());
            if(distance <= 1) {
                return Cell::Target;
            }
        }
        return Cell::White;
    };
}

auto GetCompareGivingAmmo(const vector<Player*>& teamMembers){
    return [teamMembers](Cell* a,Cell* b){
        auto aLocation = a->getLocation();
        auto bLocation = b->getLocation();
        double aMinDistance = 0, bMinDistance = 0;
        for(auto& member : teamMembers){
            double distance = member->GetLocation().ManhattanDistance(aLocation);
            if(distance < aMinDistance){
                aMinDistance = distance;
            }
            distance = member->GetLocation().ManhattanDistance(bLocation);
            if(distance < bMinDistance){
                bMinDistance = distance;
            }
        }
        return (aMinDistance + a->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)a->getLocation().x][(int)a->getLocation().y]+1) >
               (bMinDistance + b->GetLength()) * (Game::GetInstance().BOARD_SECURITY[(int)b->getLocation().x][(int)b->getLocation().y]+1);
    };
}

Support::Support(int x, int y, int team) : Player(x,y,team,FOLLOWING){
    stateMachine.AddTransition(FOLLOWING,COLLECTING_HEALTH,FollowingToCollectingHealth);
    stateMachine.AddTransition(FOLLOWING,COLLECTING_AMMO,FollowingToCollectingAmmo);
    stateMachine.AddTransition(FOLLOWING,GIVING_AMMO,FollowingToGivingAmmo);
    stateMachine.AddTransition(COLLECTING_HEALTH,FOLLOWING,CollectingHealthToFollowing);
    stateMachine.AddTransition(COLLECTING_AMMO,FOLLOWING,CollectingAmmoToFollowing);
    stateMachine.AddTransition(GIVING_AMMO,FOLLOWING,GivingAmmoToFollowing);
    stateMachine.AddTransition(COLLECTING_AMMO,COLLECTING_HEALTH,CollectingAmmoToCollectingHealth);

    AddHandler(FOLLOWING,HandelFollowing);
    AddHandler(COLLECTING_AMMO,HandelCollectingAmmo);
    AddHandler(GIVING_AMMO,HandelGivingAmmo);
}

Support &Support::operator+=(AmmoBox &box) {
    box.TakeAmmo(this);
    return *this;
}

void Support::_Tick(vector<Entity *> &entities) {
    Player::_Tick(entities);
}

bool Support::FollowingToCollectingHealth(Player *player) {
    bool healthBoxes = !Game::GetInstance().GetEntitiesOfType<HealthBox *>().empty();
    return player->GetHealth() < PLAYER_CRITICAL_HELATH && !healthBoxes;
}

bool Support::FollowingToCollectingAmmo(Player *player) {
    return player->NeedAmmo();
}

bool Support::FollowingToGivingAmmo(Player *player) {
    if(player->GetNumOfBullets() <= 0 || player->GetNumOfGrenades() <= 0){
        return false;
    }
    return !Game::GetInstance().GetEntitiesOfType<Player*>([player](Player* t){
        return (t != player) && (t->GetTeam() == player->GetTeam()) && t->NeedAmmo();
    }).empty();
}

bool Support::CollectingHealthToFollowing(Player *player) {
    bool healthBoxes = !Game::GetInstance().GetEntitiesOfType<HealthBox*>().empty();
    return (player->GetHealth() >= (int)(PLAYER_MAX_HEALTH*0.9f)) || !healthBoxes;
}

bool Support::CollectingAmmoToFollowing(Player *player) {
    bool ammoBoxes = !Game::GetInstance().GetEntitiesOfType<AmmoBox*>().empty();
    return !player->NeedAmmo() && ammoBoxes;
}

bool Support::GivingAmmoToFollowing(Player *player) {
    return !FollowingToGivingAmmo(player);
}

bool Support::CollectingAmmoToCollectingHealth(Player *player) {
    return FollowingToCollectingHealth(player);
}

void Support::HandelFollowing(Player *player) {
    auto Compare = GetCompareFollowing(Game::GetInstance().GetTeamMembers(player));

    auto Predicate = GetPredicateFollowing(player);
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}

int Support::GrenadesNeeded(const vector<AmmoBox*>& ammoBoxes)const{
    int grenadesNeeded = 0;
    auto members = Game::GetInstance().GetTeamMembers(this);
    for(auto& member : members){
        grenadesNeeded += PLAYER_MAX_GRENADES - member->GetNumOfGrenades();
    }
    return grenadesNeeded;
}

int Support::BulletsNeeded(const vector<AmmoBox*>& ammoBoxes)const{
    int bulletsNeeded = 0;
    auto members = Game::GetInstance().GetTeamMembers(this);
    for(auto& member : members){
        bulletsNeeded += PLAYER_MAX_BULLETS - member->GetNumOfBullets();
    }
    return bulletsNeeded;
}

void Support::HandelCollectingAmmo(Player *player) {
//    printf("Collecting Ammo\n");
    auto ammoBoxes = Game::GetInstance().GetEntitiesOfType<AmmoBox*>();
    for(auto& ammoBox : ammoBoxes){
        if(player->GetLocation().Distance(ammoBox->GetLocation()) <= 1){
            ammoBox->TakeAmmo(player);
//            printf("Took Ammo -- left g:%d,b%d\n",ammoBox->GetNumOfGrenades(),ammoBox->GetNumOfBullets());
            return;
        }
    }
    auto Compare = GetCompareCollectingAmmo(ammoBoxes);
    auto Predicate = GetPredicateCollectingAmmo();
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}

void Support::HandelGivingAmmo(Player *player) {
    auto ammoBoxes = Game::GetInstance().GetEntitiesOfType<AmmoBox*>();
    auto teamMembers = Game::GetInstance().GetTeamMembers(player);
    auto* support = dynamic_cast<Support*>(player);
    if(!support) {
        return;
    }
//    int grenadesNeeded = support->GrenadesNeeded(ammoBoxes);
//    int bulletsNeeded = support->BulletsNeeded(ammoBoxes);
    for(auto& member : teamMembers){
        if(member->NeedAmmo() && member->GetLocation().Distance(player->GetLocation()) <= 1.0){
            int bulletsToGive = std::min(PLAYER_MAX_BULLETS - member->GetNumOfBullets(),player->GetNumOfBullets());
            int grenadesToGive = std::min(PLAYER_MAX_GRENADES - member->GetNumOfGrenades(),player->GetNumOfGrenades());
            member->AddAmmo(bulletsToGive,grenadesToGive);
            support->TakeAmmo(bulletsToGive,grenadesToGive);
            return;
        }
    }
    auto Compare = GetCompareGivingAmmo(teamMembers);
    auto Predicate = GetPredicateGivingAmmo(player);
    AStar<decltype(Compare), decltype(Predicate)> aStar(player->GetLocation(),Compare,Predicate);
    Point2D result(0,0);
    if(aStar.Run(result)) {
        player->SetNextLocation(result);
    }
}
