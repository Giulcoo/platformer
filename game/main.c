#include "platform.h"

extern int run(GameSettings settings);

int main(void)
{
    struct World Level_1 = { "Level 1", {100,-50}, NULL, 3 };
    struct World Level_2 = { "Level 2", {100,-50}, NULL, 5 };
    struct World Level_3 = { "Level 3", {50,-40}, NULL, 8 };

    Level_1.objects = (GameObject[]){
        {BLOCK, {0,0,300,200}, BROWN, NULL},
        {BLOCK, {350,-50,400,200}, BROWN, NULL},
        {FINISH, {700,-130,40,80}, BLUE, &Level_2},
    };

    Level_2.objects = (GameObject[]){
        {BLOCK, {0,0,300,200}, BROWN, NULL},
        {BLOCK, {350,-50,400,200}, BROWN, NULL},
        {CHECKPOINT, {700,-80,20,20}, GOLD, NULL},
        {BLOCK, {850,-20,400,200}, BROWN, NULL},
        {FINISH, {1200,-100,40,80}, BLUE, &Level_3},
    };

    Level_3.objects = (GameObject[]){
        {BLOCK, {0,0,500,200}, BROWN, NULL},
        {SPIKE, {300,-40,20,20}, RED, NULL},
        {CHECKPOINT, {400,-50,20,20}, GOLD, NULL},
        {PLATFORM, {100,-100,100,5}, GRAY, NULL},
        {PLATFORM, {200,-200,100,5}, GRAY, NULL},
        {PLATFORM, {300,-300,100,5}, GRAY, NULL},
        {BLOCK, {500,-30,300,200}, BROWN, NULL},
        {FINISH, {700,-110,40,80}, BLUE, &Level_1},
    };

    return run((GameSettings) {
        "Title 123", 1600, 800, 1.300f,
        &Level_1, {20, 40}
    });
}