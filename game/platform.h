#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib/include/raylib.h"
#include "raylib/include/raymath.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

// Constants
#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_MAX_FALLING_SPD 400
#define PLAYER_OFF_MAP_HEIGHT 300

// Structs
typedef enum TouchEvent {
    NO_TOUCH,
    LEFT_TOUCH,
    RIGH_TOUCH,
    LEFT_AND_RIGHT_TOUCH,
    BOTTOM_TOUCH,
    BOTTOM_AND_LEFT_TOUCH,
    BOTTOM_AND_RIGHT_TOUCH,
    TOP_TOUCH
} TouchEvent;

typedef struct Player {
    Rectangle rect;
    Vector2 spawnPoint;
    float speed;
    bool canJump;
    int moveDir;
    int collision;
} Player;

typedef enum GameObjectType {
    BLOCK, PLATFORM, CHECKPOINT, FINISH, SPIKE
} GameObjectType;

typedef struct GameObject GameObject;
typedef struct World World;

struct GameObject {
    GameObjectType type;
    Rectangle rect;
    Color color;
    World *link;
};

typedef struct ScreenSettings {
    char* title;
    float width;
    float height;
    float zoom;
} ScreenSettings;

typedef struct PlayerSettings {
    World *startWorld;
    Vector2 size;
} PlayerSettings;

struct World {
    char* name;
    Vector2 start;
    GameObject *objects;
    int sizeObjects;
};

typedef struct GameSettings {
    ScreenSettings screen;
    PlayerSettings player;
} GameSettings;

// Function declarations
int run(GameSettings settings);
void DrawWorld(World world);
void PlayerInput(Player *player, float delta, float width, float height);
int TouchInput(float width, float height);
int TouchSide(Vector2 touchPoint, float width, float height);
void PlayerCollision(Player *player, World *world, Camera2D *camera, float delta);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height);

#endif // GAME_H
