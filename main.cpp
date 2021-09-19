#include <iostream>
#include "Game.h"
#include "Support.h"
#include "GL/glut.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Random.h"
#include "AStar.h"

#define SCREEN_WIDTH    600
#define SCREEN_HEIGHT   600

using namespace std;


int winningTeam = -1;

static void InitGL(int argc, char* argv[]);
static void Idle();
static void Display();
static void DrawBoard();
static void DrawSquare(int x, int y, Entity* entity);
static void DrawGameDetails();

int main(int argc, char* argv[]) {
//    auto compare = [](Cell*,Cell*){return false;};
//    auto predicate = [](Entity*){return Cell::Origin;};
//    AStar<decltype(compare), decltype(predicate)> aStar(nullptr,compare,predicate);
//    Point2D point(0,0);
//    aStar.Run(point);
    InitGL(argc,argv);
    return 0;
}

void InitGL(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("AI CoD");

    Game::GetInstance().Init();

    glutDisplayFunc(Display); // display is window refresh function
    glutIdleFunc(Idle);  // idle is background function

    glutMainLoop();
}

void Idle(){
    if(Game::GetInstance().IsRunning()){
        winningTeam = Game::GetInstance().Tick();
    }
    glutPostRedisplay();
}

void Display(){
    glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

    DrawBoard();
    DrawGameDetails();
    if(winningTeam){
        const char* text = winningTeam == 1 ? "Team 1 Wins!" : "Team 2 Wins!";
        glColor3d(1,0,0);
        glRasterPos2d(0,0);
        for(int i = 0 ; i < strlen(text) ; i++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,text[i]);
        }
    }

    glutSwapBuffers(); // show all
}

void DrawBoard(){
    Game& game = Game::GetInstance();
    for(int i = 0 ; i < GAME_WIDTH ; i++){
        for(int j = 0 ; j < GAME_HEIGHT ; j++){
            DrawSquare(i,j,game.BOARD[i][j]);
        }
    }
}

void DrawSquare(int x, int y, Entity* entity){
    int xsz = SCREEN_WIDTH / GAME_WIDTH;
    int ysz = SCREEN_HEIGHT / GAME_HEIGHT;
    auto& game = Game::GetInstance();
    double x_r, y_r;
    if(dynamic_cast<Wall*>(entity)){
        glColor3d(0.3, 0.3, 0.3);
    }else if(dynamic_cast<Path*>(entity)){
        glColor3d(0.7, 0.7, 0.7);
//        glColor3d(1-game.BOARD_SECURITY[x][y], 1-game.BOARD_SECURITY[x][y], 1-game.BOARD_SECURITY[x][y]);
    }else if(dynamic_cast<Fighter*>(entity)){
        if(entity->GetTeam() == 0)
            glColor3d(0, 0, 0.8);
        else
            glColor3d(0.8, 0, 0);
    }else if(dynamic_cast<Support*>(entity)){
        glColor3d(0.5,0.5,0);
    }else if(dynamic_cast<Grenade*>(entity)){
        glColor3d(0, 0.8, 0);
    }else if(dynamic_cast<Bullet*>(entity)) {
        glColor3d(0, 0, 0);
    }else if(dynamic_cast<HealthBox*>(entity)){
        glColor3d(0.4,0,0);
    }else if(dynamic_cast<AmmoBox*>(entity)) {
        glColor3d(0,0.4,0);
    }else{
        return;
    }
    x_r = 2.0 * (x * xsz) / SCREEN_WIDTH - 1; // value in range [-1,1)
    y_r = 2.0 * (y * ysz) / SCREEN_HEIGHT - 1; // value in range [-1,1)
    glBegin(GL_POLYGON);
    glVertex2d(x_r, y_r);
    glVertex2d(x_r, y_r+(2.0*ysz)/SCREEN_HEIGHT);
    glVertex2d(x_r+2.0*xsz/SCREEN_WIDTH, y_r + (2.0 * ysz) / SCREEN_HEIGHT);
    glVertex2d(x_r + 2.0 * xsz / SCREEN_WIDTH, y_r);
    glEnd();
}

static char TEXT_BUFFER[512];

static void DrawText(const char* text, double x, double y){
    glRasterPos2d(x,y);
    for(int i = 0 ; i < strlen(text) ; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
}

static void DrawPlayerDetails(Player* player, double x, double y){
    if(dynamic_cast<Support*>(player)) {
        glColor3d(0.5,0.5,0.05);
    }else{
        glColor3d(0,0.4,0);
    }
    sprintf((char*)TEXT_BUFFER,"H:%03d%% B:%d G:%d",player->GetHealth(),player->GetNumOfBullets(),player->GetNumOfGrenades());
    DrawText(TEXT_BUFFER,x,y);
}

static void DrawGameDetails(){
    auto team1 = Game::GetInstance().GetTeamMembers(0);
    auto team2 = Game::GetInstance().GetTeamMembers(1);
    double height = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_10,'T')+30;
    double y = 1 - (height+15)/SCREEN_HEIGHT;
    glColor3d(0,0,0.8);
    DrawText("Team 1",-0.99,y);
    y -= height/SCREEN_HEIGHT;
    for(auto& member : team1){
        DrawPlayerDetails(member,-0.99,y);
        y -= height/SCREEN_HEIGHT;
    }
    y = 1 - (height+15)/SCREEN_HEIGHT;
    glColor3d(0.8,0,0);
    DrawText("Team 2",0.55,y);
    y -= height/SCREEN_HEIGHT;
    for(auto& member : team2){
        DrawPlayerDetails(member,0.55,y);
        y -= height/SCREEN_HEIGHT;
    }
}
