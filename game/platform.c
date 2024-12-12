#include "platform.h"

int run(GameSettings settings)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if(settings.screen.width < 0 || settings.screen.height < 0) {
        settings.screen.width = GetMonitorWidth(GetCurrentMonitor());
        settings.screen.height = GetMonitorWidth(GetCurrentMonitor());
    }
    InitWindow(settings.screen.width, settings.screen.height, settings.screen.title);

    Player player = { 0 };
    player.rect = (Rectangle) { 0, 0, settings.player.size.x , settings.player.size.y };
    player.speed = 0;
    player.canJump = false;
    player.collision = 0;

    World currentWorld = *settings.player.startWorld;
    
    player.spawnPoint = currentWorld.start;
    player.rect.x = player.spawnPoint.x;
    player.rect.y = player.spawnPoint.y;

    Camera2D camera = { 0 };
    camera.target = (Vector2) { player.rect.x, player.rect.y };
    camera.offset = (Vector2){ settings.screen.width/2.0f, settings.screen.height/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = settings.screen.zoom;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        PlayerInput(&player, deltaTime, settings.screen.width, settings.screen.height);
        PlayerCollision(&player, &currentWorld, &camera, deltaTime);

        UpdateCameraCenterSmoothFollow(&camera, &player, deltaTime, settings.screen.width, settings.screen.height);

        BeginDrawing();

            ClearBackground(SKYBLUE);

            BeginMode2D(camera);

                DrawWorld(currentWorld);
                DrawRectangleRec(player.rect, DARKPURPLE);

            EndMode2D();

            DrawText("World:", 20, 20, 35, BLACK);
            DrawText(currentWorld.name, 130, 20, 35, BLACK);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void DrawWorld(World world){
    for (int i = 0; i < world.sizeObjects; i++){
        switch(world.objects[i].type){
            case SPIKE:
                float spikeX = world.objects[i].rect.x + world.objects[i].rect.width/2.0f;
                float spikeSize =  world.objects[i].rect.width*(2.0f/3.0f);
                DrawRectanglePro((Rectangle) { spikeX, world.objects[i].rect.y, spikeSize, spikeSize  }, (Vector2) {0,0}, 45, world.objects[i].color);
                break;
            default:
                DrawRectangleRec(world.objects[i].rect, world.objects[i].color);
        }
    } 
}

void PlayerInput(Player *player, float delta, float width, float height)
{
    // Read inputs
    int touchInput = TouchInput(width, height);

    bool jumpPressed = IsKeyDown(KEY_SPACE) || (touchInput >= BOTTOM_TOUCH && touchInput <= BOTTOM_AND_RIGHT_TOUCH);
    bool leftPressed = IsKeyDown(KEY_A) || touchInput == LEFT_TOUCH || touchInput == LEFT_AND_RIGHT_TOUCH || touchInput == BOTTOM_AND_LEFT_TOUCH;
    bool rightPressed = IsKeyDown(KEY_D) || touchInput == RIGH_TOUCH || touchInput == LEFT_AND_RIGHT_TOUCH || touchInput == BOTTOM_AND_RIGHT_TOUCH;
    bool respawnPressed = IsKeyPressed(KEY_R) || touchInput == TOP_TOUCH;

    // Do actions depending on inputs
    if (leftPressed && player->collision >= 0) {
        player->rect.x -= PLAYER_HOR_SPD*delta;
        player->moveDir = -1;
    }

    if (rightPressed && player->collision <= 0) {
        player->rect.x += PLAYER_HOR_SPD*delta;
        player->moveDir = 1;
    }

    if (leftPressed == rightPressed) {
        player->moveDir = 0;
    }
    
    if (jumpPressed && (player->canJump 
        || (player->collision < 0 && rightPressed) 
        || (player->collision > 0 && leftPressed)))
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->rect.y -= 1;
        player->canJump = false;
    }

    if (respawnPressed)
    {
        player->rect.x = player->spawnPoint.x;
        player->rect.y = player->spawnPoint.y;
        player->speed = 0;
    }
}

int TouchInput(float width, float height)
{
    int touchInput = NO_TOUCH;
    int touchCount = GetTouchPointCount();

    if(touchCount > 0){
        touchInput = TouchSide(GetTouchPosition(touchCount-1), width, height);
    }

    // Combinational input
    if(touchCount > 1){
        int secondInput = TouchSide(GetTouchPosition(touchCount-2), width, height);

        if ((touchInput == BOTTOM_TOUCH && secondInput == LEFT_TOUCH)
        || (touchInput == LEFT_TOUCH && secondInput == BOTTOM_TOUCH))
        {
            touchInput = BOTTOM_AND_LEFT_TOUCH;
        }   
        else if ((touchInput == BOTTOM_TOUCH && secondInput == RIGH_TOUCH)
        || (touchInput == RIGH_TOUCH && secondInput == BOTTOM_TOUCH))
        {
            touchInput = BOTTOM_AND_RIGHT_TOUCH;
        }   
        else if ((touchInput == LEFT_TOUCH && secondInput == RIGH_TOUCH)
        || (touchInput == RIGH_TOUCH && secondInput == LEFT_TOUCH))
        {
            touchInput = LEFT_AND_RIGHT_TOUCH;
        }   
    }

    return touchInput;
}

int TouchSide(Vector2 touchPoint, float width, float height){
    if(touchPoint.y > height - height/5){
        return BOTTOM_TOUCH;
    }
    else if(touchPoint.y < height/10){
        return TOP_TOUCH;
    }
    else if(touchPoint.x < width/2){
        return LEFT_TOUCH;
    }
    else{
        return RIGH_TOUCH;
    }
}

void PlayerCollision(Player *player, World *world, Camera2D *camera, float delta)
{
    Vector2 posTopLeft = (Vector2) { player->rect.x, player->rect.y };
    Vector2 posBottomRight = (Vector2) { player->rect.x + player->rect.width, player->rect.y + player->rect.height };

    bool bottomObstacle = false;
    bool sideObstacle = false;
    bool playerRespawn = false;

    for (int i = 0; i < world->sizeObjects; i++)
    {
        GameObject obj = world->objects[i];
        Rectangle rect = obj.rect;
        
        switch(obj.type){
            case PLATFORM:
                if (rect.x <= posBottomRight.x &&
                    rect.x + rect.width >= posTopLeft.x &&
                    rect.y >= posBottomRight.y &&
                    rect.y <= posBottomRight.y + player->speed*delta)
                {
                    bottomObstacle = true;
                    player->canJump = true;
                    player->speed = 0.0f;
                    player->rect.y = obj.rect.y - player->rect.height;
                }
                break;
            case BLOCK:
                if (CheckCollisionRecs(player->rect, rect))
                {
                    if (posBottomRight.x - player->rect.width/2 < rect.x) // Left collision
                    {
                        sideObstacle = true;
                        player->collision = 1;
                        player->rect.x = rect.x - player->rect.width + 1;
                    } 
                    else if (posTopLeft.x + player->rect.width/2 > rect.x + rect.width) // Right collision
                    {
                        sideObstacle = true;
                        player->collision = -1;
                        player->rect.x = rect.x + rect.width - 1;
                    }
                    else if(posBottomRight.y - player->rect.height/4 < rect.y) // Bottom collision
                    {
                        bottomObstacle = true;
                        player->canJump = true;
                        player->speed = 0.0f;
                        player->rect.y = rect.y - player->rect.height + 1;
                    }
                    else if(posTopLeft.y + player->rect.height > rect.y + rect.height) // Top collision
                    {
                        player->speed = 0.0f;
                        player->rect.y = rect.y + rect.height;
                    }
                }
                break;
            case CHECKPOINT:
                if (CheckCollisionRecs(player->rect, rect))
                {
                    player->spawnPoint = (Vector2) 
                    { 
                        rect.x + rect.width/2 - player->rect.width/2, 
                        rect.y + rect.height/2 - player->rect.height/2 
                    };
                }
                break;
            case FINISH:
                if (CheckCollisionRecs(player->rect, rect))
                {
                    *world = *obj.link;
                    camera->target = world->start; 
                    player->spawnPoint = world->start;
                    playerRespawn = true;
                }
                break;
            case SPIKE:
                if (CheckCollisionRecs(player->rect, rect)) playerRespawn = true;
                break;
        }
    }

    if(player->rect.y > PLAYER_OFF_MAP_HEIGHT) playerRespawn = true;

    if (!bottomObstacle)
    {
        player->rect.y += player->speed*delta;
        player->speed += G*delta;
        if(player->speed > PLAYER_MAX_FALLING_SPD) player->speed = PLAYER_MAX_FALLING_SPD;
        player->canJump = false;
    }

    if (!sideObstacle) 
    {
        player->collision = 0;
    }

    if(playerRespawn){
        player->rect.x = player->spawnPoint.x;
        player->rect.y = player->spawnPoint.y;
    }
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;
    Vector2 lookAhead = (Vector2) { (100)*player->moveDir, player->speed };

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    Vector2 diff = Vector2Subtract(Vector2Add((Vector2) {player->rect.x, player->rect.y}, lookAhead), camera->target);
    float length = Vector2Length(diff);

    if (fabsf(camera->target.y - player->rect.y) >= height/2) 
    {
        camera->target = (Vector2) {player->rect.x, player->rect.y}; 
    }
    else if (length > minEffectLength) 
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
} 